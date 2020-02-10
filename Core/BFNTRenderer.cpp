#include "BFNTRenderer.h"
#include "BFNTLoader.h"

#include "Material.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "UTF8.h"

using std::string;
using std::make_unique;

#define SAFE_DELETE(a) if (a) { delete a; a = nullptr; }
#define SAFE_RELEASE(a) if (a) { a->Release(); a = nullptr; }

CBFNTRenderer::CBFNTRenderer(ID3D11Device* const PtrDevice, ID3D11DeviceContext* const PtrDeviceContext) :
	m_PtrDevice{ PtrDevice }, m_PtrDeviceContext{ PtrDeviceContext }
{
	assert(m_PtrDevice);
	assert(m_PtrDeviceContext);
}

CBFNTRenderer::~CBFNTRenderer()
{
	ReleaseResources();
}

void CBFNTRenderer::Create(const std::string& BFNT_FileName, const DirectX::XMFLOAT2& WindowSize)
{
	ReleaseResources();

	string _BFNT_FileName{ BFNT_FileName };
	for (auto& ch : _BFNT_FileName)
	{
		if (ch == '//') ch = '\\';
	}

	string Dir{ _BFNT_FileName };
	string FileNameOnly{ _BFNT_FileName };
	{
		size_t At{ Dir.find_last_of('\\') };
		if (At != string::npos)
		{
			Dir = Dir.substr(0, At + 1);
		}
		else
		{
			Dir.clear();
		}

		FileNameOnly = FileNameOnly.substr(Dir.size());

		size_t Ext{ FileNameOnly.find_last_of('.') };
		if (Ext != string::npos)
		{
			FileNameOnly = FileNameOnly.substr(0, Ext);
		}
	}

	m_bHasOwnData = true; // @important

	{
		m_BFNTLoader = new CBFNTLoader();
		m_BFNTLoader->Load(BFNT_FileName.c_str());

		m_FontTexture = new CTexture(m_PtrDevice, m_PtrDeviceContext);
		m_FontTexture->CreateTextureFromFile(Dir + FileNameOnly + ".png", false);

		m_WindowSize = WindowSize;
		m_CBSpaceData.ProjectionMatrix = XMMatrixOrthographicLH(m_WindowSize.x, m_WindowSize.y, 0.0f, 1.0f);

		m_VSFont = new CShader(m_PtrDevice, m_PtrDeviceContext);
		m_VSFont->Create(EShaderType::VertexShader, CShader::EVersion::_4_0, true, L"Shader\\VSFont.hlsl", "main",
			KInputLayout, ARRAYSIZE(KInputLayout));

		m_PSFont = new CShader(m_PtrDevice, m_PtrDeviceContext);
		m_PSFont->Create(EShaderType::PixelShader, CShader::EVersion::_4_0, true, L"Shader\\PSFont.hlsl", "main");

		m_CBSpace = new CConstantBuffer(m_PtrDevice, m_PtrDeviceContext, &m_CBSpaceData, sizeof(m_CBSpaceData));
		m_CBSpace->Create();

		m_CBColor = new CConstantBuffer(m_PtrDevice, m_PtrDeviceContext, &m_CBColorData, sizeof(m_CBColorData));
		m_CBColor->Create();

		m_CommonStates = new CommonStates(m_PtrDevice);
	}

	m_vGlyphVertices.resize(1);
	m_vGlyphIndices.resize(1);

	CreateBuffers();
}

void CBFNTRenderer::Create(const CBFNTRenderer* const FontRenderer)
{
	ReleaseResources();

	m_bHasOwnData = false; // @important

	{
		m_BFNTLoader = FontRenderer->m_BFNTLoader;

		m_FontTexture = FontRenderer->m_FontTexture;

		m_WindowSize = FontRenderer->m_WindowSize;
		m_CBSpaceData.ProjectionMatrix = XMMatrixOrthographicLH(m_WindowSize.x, m_WindowSize.y, 0.0f, 1.0f);

		m_VSFont = FontRenderer->m_VSFont;

		m_PSFont = FontRenderer->m_PSFont;

		m_CBSpace = FontRenderer->m_CBSpace;

		m_CBColor = FontRenderer->m_CBColor;

		m_CommonStates = FontRenderer->m_CommonStates;
	}

	m_vGlyphVertices.resize(1);
	m_vGlyphIndices.resize(1);

	CreateBuffers();
}

void CBFNTRenderer::CreateBuffers()
{
	// Vertex buffer
	{
		D3D11_BUFFER_DESC BufferDesc{};
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		BufferDesc.ByteWidth = static_cast<UINT>(sizeof(SVertex) * m_StringCapacity * 4);
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		BufferDesc.MiscFlags = 0;
		BufferDesc.StructureByteStride = 0;
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;

		D3D11_SUBRESOURCE_DATA SubresourceData{};
		SubresourceData.pSysMem = &m_vGlyphVertices[0];

		SAFE_RELEASE(m_VertexBuffer);
		m_PtrDevice->CreateBuffer(&BufferDesc, &SubresourceData, &m_VertexBuffer);
	}

	// Index buffer
	{
		D3D11_BUFFER_DESC BufferDesc{};
		BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		BufferDesc.ByteWidth = static_cast<UINT>(sizeof(UINT) * m_StringCapacity * 6);
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		BufferDesc.MiscFlags = 0;
		BufferDesc.StructureByteStride = 0;
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;

		D3D11_SUBRESOURCE_DATA SubresourceData{};
		SubresourceData.pSysMem = &m_vGlyphIndices[0];

		SAFE_RELEASE(m_IndexBuffer);
		m_PtrDevice->CreateBuffer(&BufferDesc, &SubresourceData, &m_IndexBuffer);
	}
}

void CBFNTRenderer::UpdateBuffers()
{
	if (m_vGlyphVertices.empty()) return;
	
	size_t GlyphCount{ m_vGlyphVertices.size() / 4 };
	D3D11_MAPPED_SUBRESOURCE MappedSubresource{};

	// Vertex buffer
	if (SUCCEEDED(m_PtrDeviceContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource)))
	{
		memcpy(MappedSubresource.pData, &m_vGlyphVertices[0], sizeof(SVertex) * 4 * GlyphCount);
		m_PtrDeviceContext->Unmap(m_VertexBuffer, 0);
	}

	// Index buffer
	if (SUCCEEDED(m_PtrDeviceContext->Map(m_IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource)))
	{
		memcpy(MappedSubresource.pData, &m_vGlyphIndices[0], sizeof(UINT) * 6 * GlyphCount);
		m_PtrDeviceContext->Unmap(m_IndexBuffer, 0);
	}
}

void CBFNTRenderer::ReleaseResources()
{
	SAFE_RELEASE(m_VertexBuffer);
	SAFE_RELEASE(m_IndexBuffer);

	if (m_bHasOwnData)
	{
		SAFE_DELETE(m_BFNTLoader);
		SAFE_DELETE(m_FontTexture);
		SAFE_DELETE(m_VSFont);
		SAFE_DELETE(m_PSFont);
		SAFE_DELETE(m_CBSpace);
		SAFE_DELETE(m_CBColor);
		SAFE_DELETE(m_CommonStates);
	}
}

void CBFNTRenderer::SetVSConstantBufferSlot(UINT Slot)
{
	m_CBSpaceSlot = Slot;
}

void CBFNTRenderer::SetPSConstantBufferSlot(UINT Slot)
{
	m_CBColorSlot = Slot;
}

void CBFNTRenderer::PushGlyph(size_t BufferCharIndex, size_t GlyphIndex, int32_t& CursorX, int32_t CursorY)
{
	const auto& Data{ m_BFNTLoader->GetData() };
	const auto& Glyph{ Data.vGlyphs[GlyphIndex] };

	float X0{ static_cast<float>(CursorX + Glyph.XBearing) };
	float Y0{ static_cast<float>(CursorY + Glyph.YBearing) };
	float X1{ X0 + static_cast<float>(Glyph.Width) };
	float Y1{ Y0 - static_cast<float>(Glyph.Height) };

	size_t VertexBase{ BufferCharIndex * 4 };
	m_vGlyphVertices[VertexBase + 0] = SVertex(X0, Y0, Glyph.U0, Glyph.V0);
	m_vGlyphVertices[VertexBase + 1] = SVertex(X1, Y0, Glyph.U1, Glyph.V0);
	m_vGlyphVertices[VertexBase + 2] = SVertex(X0, Y1, Glyph.U0, Glyph.V1);
	m_vGlyphVertices[VertexBase + 3] = SVertex(X1, Y1, Glyph.U1, Glyph.V1);

	size_t IndexBase{ BufferCharIndex * 6 };
	m_vGlyphIndices[IndexBase + 0] = static_cast<UINT>(VertexBase + 0);
	m_vGlyphIndices[IndexBase + 1] = static_cast<UINT>(VertexBase + 1);
	m_vGlyphIndices[IndexBase + 2] = static_cast<UINT>(VertexBase + 2);
	m_vGlyphIndices[IndexBase + 3] = static_cast<UINT>(VertexBase + 1);
	m_vGlyphIndices[IndexBase + 4] = static_cast<UINT>(VertexBase + 3);
	m_vGlyphIndices[IndexBase + 5] = static_cast<UINT>(VertexBase + 2);

	CursorX += Glyph.XAdvance; // @important
}

void CBFNTRenderer::RenderString(const char* UTF8String, const DirectX::XMFLOAT2& Position, const DirectX::XMFLOAT4& Color)
{
	assert(UTF8String);

	size_t Hash{ std::hash<const char*>{}(UTF8String) };
	if (Hash != m_PrevHash)
	{
		m_PrevHash = Hash;

		size_t Length{ GetUTF8StringLength(UTF8String) };
		m_vGlyphVertices.resize(Length * 4);
		m_vGlyphIndices.resize(Length * 6);

		size_t BufferCharIndex{};
		size_t BufferAt{};
		size_t BufferSize{ strlen(UTF8String) };
		int32_t CursorX{ static_cast<int32_t>(-m_WindowSize.x * 0.5f) };
		int32_t CursorY{ static_cast<int32_t>(+m_WindowSize.y * 0.5f) };
		const auto& GlyphIDToIndexMap{ m_BFNTLoader->GetData().umapGlyphIDtoIndex };
		while (BufferAt < BufferSize)
		{
			size_t ByteCount{ GetUTF8CharacterByteCount(UTF8String[BufferAt]) };

			UUTF8_ID UTF8{};
			memcpy(UTF8.Chars, &UTF8String[BufferAt], ByteCount);

			size_t GlyphIndex{ (GlyphIDToIndexMap.find(UTF8.ID) != GlyphIDToIndexMap.end()) ? GlyphIDToIndexMap.at(UTF8.ID) : 0 };
			PushGlyph(BufferCharIndex, GlyphIndex, CursorX, CursorY);

			++BufferCharIndex;
			BufferAt += ByteCount;
		}

		UpdateStringCapacity();
	}

	m_CBSpaceData.Position.x = Position.x;
	m_CBSpaceData.Position.y = Position.y;
	m_CBSpace->Update();

	m_CBColorData = Color;
	m_CBColor->Update();

	Render();
}

void CBFNTRenderer::Render()
{
	m_CBSpace->Use(EShaderType::VertexShader, m_CBSpaceSlot);
	m_CBColor->Use(EShaderType::PixelShader, m_CBColorSlot);

	m_FontTexture->Use();

	m_VSFont->Use();
	m_PSFont->Use();

	ID3D11SamplerState* Samplers[]{ m_CommonStates->PointClamp() };
	m_PtrDeviceContext->PSSetSamplers(0, 1, Samplers);

	m_PtrDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_PtrDeviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &m_VertexBufferStride, &m_VertexBufferOffset);
	m_PtrDeviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	m_PtrDeviceContext->DrawIndexed(static_cast<UINT>(m_vGlyphIndices.size()), 0, 0);
}

bool CBFNTRenderer::UpdateStringCapacity()
{
	bool bCapacityChanged{ false };
	size_t GlyphCount{ (m_vGlyphVertices.size() / 4) };
	while (GlyphCount > m_StringCapacity)
	{
		m_StringCapacity *= 2;
		bCapacityChanged = true;
	}
	if (m_StringCapacity > KMaxStringCapacity)
	{
		m_StringCapacity = KMaxStringCapacity;
	}

	if (bCapacityChanged)
	{
		CreateBuffers();
	}
	else
	{
		UpdateBuffers();
	}
	return bCapacityChanged;
}
