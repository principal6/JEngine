#include "BMFontRenderer.h"
#include "BMFont.h"
#include "Material.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "UTF8.h"

using std::make_unique;

#define SAFE_DELETE(a) if (a) { delete a; a = nullptr; }

CBMFontRenderer::CBMFontRenderer(ID3D11Device* const PtrDevice, ID3D11DeviceContext* const PtrDeviceContext) :
	m_PtrDevice{ PtrDevice }, m_PtrDeviceContext{ PtrDeviceContext }
{
	assert(m_PtrDevice);
	assert(m_PtrDeviceContext);
}

CBMFontRenderer::~CBMFontRenderer()
{
	ReleaseResources();
}

void CBMFontRenderer::Create(const std::string& FNT_FileName, const DirectX::XMFLOAT2& WindowSize)
{
	ReleaseResources();

	m_bHasOwnData = true; // @important

	// These variables needs to be updated every time
	{
		m_BMFont = new CBMFont();
		m_BMFont->Load(FNT_FileName);

		const auto& FontData{ m_BMFont->GetData() };
		m_TextureSizeDenominator.x = 1.0f / static_cast<float>(FontData.Common.ScaleW);
		m_TextureSizeDenominator.y = 1.0f / static_cast<float>(FontData.Common.ScaleH);

		m_FontTexture = new CTexture(m_PtrDevice, m_PtrDeviceContext);
		m_FontTexture->CreateTextureFromFile(m_BMFont->GetData().vPages.front(), false);

		m_WindowSize = WindowSize;
		m_CBSpaceData.ProjectionMatrix = XMMatrixOrthographicLH(m_WindowSize.x, m_WindowSize.y, 0.0f, 1.0f);
	}

	// These variables only needs to be created once
	{
		m_VSFont = new CShader(m_PtrDevice, m_PtrDeviceContext);
		m_VSFont->Create(EShaderType::VertexShader, CShader::EVersion::_4_0, true, L"Shader\\VSFont.hlsl", "main",
			KInputLayout, ARRAYSIZE(KInputLayout));

		m_PSFont = new CShader(m_PtrDevice, m_PtrDeviceContext);
		m_PSFont->Create(EShaderType::PixelShader, CShader::EVersion::_4_0, true, L"Shader\\PSFont.hlsl", "main");

		m_CBSpace = new CConstantBuffer(m_PtrDevice, m_PtrDeviceContext, &m_CBSpaceData, sizeof(m_CBSpaceData));
		m_CBSpace->Create();

		m_CommonStates = new CommonStates(m_PtrDevice);
	}

	m_vGlyphVertices.resize(1);
	m_vGlyphIndices.resize(1);

	CreateBuffers();
}

void CBMFontRenderer::Create(const CBMFontRenderer* const FontRenderer)
{
	ReleaseResources();

	m_bHasOwnData = false; // @important

	m_BMFont = FontRenderer->m_BMFont;

	const auto& FontData{ m_BMFont->GetData() };
	m_TextureSizeDenominator.x = 1.0f / static_cast<float>(FontData.Common.ScaleW);
	m_TextureSizeDenominator.y = 1.0f / static_cast<float>(FontData.Common.ScaleH);

	m_FontTexture = FontRenderer->m_FontTexture;

	m_WindowSize = FontRenderer->m_WindowSize;
	m_CBSpaceData.ProjectionMatrix = XMMatrixOrthographicLH(m_WindowSize.x, m_WindowSize.y, 0.0f, 1.0f);

	m_VSFont = FontRenderer->m_VSFont;
	m_PSFont = FontRenderer->m_PSFont;
	m_CBSpace = FontRenderer->m_CBSpace;
	m_CommonStates = FontRenderer->m_CommonStates;

	m_vGlyphVertices.resize(1);
	m_vGlyphIndices.resize(1);

	CreateBuffers();
}

void CBMFontRenderer::CreateBuffers()
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

		m_VertexBuffer.Reset();
		m_PtrDevice->CreateBuffer(&BufferDesc, &SubresourceData, m_VertexBuffer.ReleaseAndGetAddressOf());
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

		m_IndexBuffer.Reset();
		m_PtrDevice->CreateBuffer(&BufferDesc, &SubresourceData, m_IndexBuffer.ReleaseAndGetAddressOf());
	}
}

void CBMFontRenderer::UpdateBuffers()
{
	if (m_vGlyphVertices.empty()) return;
	
	size_t GlyphCount{ m_vGlyphVertices.size() / 4 };
	D3D11_MAPPED_SUBRESOURCE MappedSubresource{};

	// Vertex buffer
	if (SUCCEEDED(m_PtrDeviceContext->Map(m_VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource)))
	{
		memcpy(MappedSubresource.pData, &m_vGlyphVertices[0], sizeof(SVertex) * 4 * GlyphCount);
		m_PtrDeviceContext->Unmap(m_VertexBuffer.Get(), 0);
	}

	// Index buffer
	if (SUCCEEDED(m_PtrDeviceContext->Map(m_IndexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource)))
	{
		memcpy(MappedSubresource.pData, &m_vGlyphIndices[0], sizeof(UINT) * 6 * GlyphCount);
		m_PtrDeviceContext->Unmap(m_IndexBuffer.Get(), 0);
	}
}

void CBMFontRenderer::ReleaseResources()
{
	if (m_bHasOwnData)
	{
		SAFE_DELETE(m_BMFont);
		SAFE_DELETE(m_FontTexture);
		SAFE_DELETE(m_VSFont);
		SAFE_DELETE(m_PSFont);
		SAFE_DELETE(m_CBSpace);
		SAFE_DELETE(m_CommonStates);
	}
}

void CBMFontRenderer::SetVSConstantBufferSlot(UINT Slot)
{
	m_CBSpaceSlot = Slot;
}

void CBMFontRenderer::SetFontColor(const DirectX::XMFLOAT4& Color)
{
	m_FontColor = Color;
}

void CBMFontRenderer::PushGlyph(size_t CharIndex, int32_t& CursorX, int32_t CursorY)
{
	const auto& FontData{ m_BMFont->GetData() };
	const auto& Char{ FontData.vChars[CharIndex] };

	float X0{ static_cast<float>(CursorX + Char.XOffset) };
	float Y0{ static_cast<float>(CursorY - Char.YOffset) };
	float X1{ X0 + static_cast<float>(Char.Width) };
	float Y1{ Y0 - static_cast<float>(Char.Height) };

	float U0{ static_cast<float>(Char.X) * m_TextureSizeDenominator.x };
	float V0{ static_cast<float>(Char.Y) * m_TextureSizeDenominator.y };
	float U1{ U0 + static_cast<float>(Char.Width) * m_TextureSizeDenominator.x };
	float V1{ V0 + static_cast<float>(Char.Height) * m_TextureSizeDenominator.y };

	m_vGlyphVertices.emplace_back(X0, Y0, U0, V0, m_FontColor);
	m_vGlyphVertices.emplace_back(X1, Y0, U1, V0, m_FontColor);
	m_vGlyphVertices.emplace_back(X0, Y1, U0, V1, m_FontColor);
	m_vGlyphVertices.emplace_back(X1, Y1, U1, V1, m_FontColor);

	CursorX += Char.XAdvance; // @important
}

void CBMFontRenderer::RenderMutableString(const char* UTF8String, const DirectX::XMFLOAT2& Position)
{
	assert(UTF8String);

	size_t Length{ GetUTF8StringLength(UTF8String) };
	const auto& CharIndexMap{ m_BMFont->GetCharIndexMap() };
	m_vGlyphVertices.clear();
	m_vGlyphVertices.reserve(Length * 4);
	m_vGlyphIndices.clear();
	m_vGlyphIndices.reserve(Length * 6);

	UINT GlyphIndex{};
	size_t BufferAt{};
	size_t BufferSize{ strlen(UTF8String) };
	int32_t CursorX{ static_cast<int32_t>(-m_WindowSize.x * 0.5f) };
	int32_t CursorY{ static_cast<int32_t>(+m_WindowSize.y * 0.5f) };
	while (BufferAt < BufferSize)
	{
		size_t ByteCount{ GetUTF8CharacterByteCount(UTF8String[BufferAt]) };

		UUTF8_ID UTF8{};
		memcpy(UTF8.Chars, &UTF8String[BufferAt], ByteCount);

		size_t CharIndex{ (CharIndexMap.find(UTF8.ID) != CharIndexMap.end()) ? CharIndexMap.at(UTF8.ID) : 0 };
		PushGlyph(CharIndex, CursorX, CursorY);

		UINT VertexBase{ GlyphIndex * 4 };
		m_vGlyphIndices.emplace_back(VertexBase + 0);
		m_vGlyphIndices.emplace_back(VertexBase + 1);
		m_vGlyphIndices.emplace_back(VertexBase + 2);
		m_vGlyphIndices.emplace_back(VertexBase + 1);
		m_vGlyphIndices.emplace_back(VertexBase + 3);
		m_vGlyphIndices.emplace_back(VertexBase + 2);

		++GlyphIndex;
		BufferAt += ByteCount;
	}

	UpdateStringCapacity();

	m_CBSpaceData.Position.x = Position.x;
	m_CBSpaceData.Position.y = Position.y;
	m_CBSpace->Update();

	Render();
}

void CBMFontRenderer::RenderConstantString(const char* UTF8String, const DirectX::XMFLOAT2& Position)
{
	assert(UTF8String);
	if (m_PtrConstantUTF8String != UTF8String)
	{
		m_PtrConstantUTF8String = UTF8String;

		size_t Length{ GetUTF8StringLength(m_PtrConstantUTF8String) };
		const auto& CharIndexMap{ m_BMFont->GetCharIndexMap() };
		m_vGlyphVertices.clear();
		m_vGlyphVertices.reserve(Length * 4);
		m_vGlyphIndices.clear();
		m_vGlyphIndices.reserve(Length * 6);

		UINT GlyphIndex{};
		size_t BufferAt{};
		size_t BufferSize{ strlen(m_PtrConstantUTF8String) };
		int32_t CursorX{ static_cast<int32_t>(-m_WindowSize.x * 0.5f) };
		int32_t CursorY{ static_cast<int32_t>(+m_WindowSize.y * 0.5f) };
		while (BufferAt < BufferSize)
		{
			size_t ByteCount{ GetUTF8CharacterByteCount(m_PtrConstantUTF8String[BufferAt]) };

			UUTF8_ID UTF8{};
			memcpy(UTF8.Chars, &m_PtrConstantUTF8String[BufferAt], ByteCount);

			size_t CharIndex{ (CharIndexMap.find(UTF8.ID) != CharIndexMap.end()) ? CharIndexMap.at(UTF8.ID) : 0 };
			PushGlyph(CharIndex, CursorX, CursorY);

			UINT VertexBase{ GlyphIndex * 4 };
			m_vGlyphIndices.emplace_back(VertexBase + 0);
			m_vGlyphIndices.emplace_back(VertexBase + 1);
			m_vGlyphIndices.emplace_back(VertexBase + 2);
			m_vGlyphIndices.emplace_back(VertexBase + 1);
			m_vGlyphIndices.emplace_back(VertexBase + 3);
			m_vGlyphIndices.emplace_back(VertexBase + 2);

			++GlyphIndex;
			BufferAt += ByteCount;
		}

		UpdateStringCapacity();
	}

	m_CBSpaceData.Position.x = Position.x;
	m_CBSpaceData.Position.y = Position.y;
	m_CBSpace->Update();

	Render();
}

void CBMFontRenderer::ClearRegistered()
{
	m_vGlyphVertices.clear();
	m_vGlyphIndices.clear();

	m_CBSpaceData.Position.x = 0;
	m_CBSpaceData.Position.y = 0;
	m_CBSpace->Update();
}

void CBMFontRenderer::RegisterString(const char* UTF8String, const DirectX::XMFLOAT2& Position, const DirectX::XMFLOAT4& Color)
{
	assert(UTF8String);

	m_FontColor = Color;

	size_t BufferAt{};
	size_t BufferSize{ strlen(UTF8String) };
	int32_t CursorX{ static_cast<int32_t>(-m_WindowSize.x * 0.5f + Position.x) };
	int32_t CursorY{ static_cast<int32_t>(+m_WindowSize.y * 0.5f - Position.y) };
	const auto& CharIndexMap{ m_BMFont->GetCharIndexMap() };
	while (BufferAt < BufferSize)
	{
		UINT VertexBase{ static_cast<UINT>(m_vGlyphVertices.size()) };
		m_vGlyphIndices.emplace_back(VertexBase + 0);
		m_vGlyphIndices.emplace_back(VertexBase + 1);
		m_vGlyphIndices.emplace_back(VertexBase + 2);
		m_vGlyphIndices.emplace_back(VertexBase + 1);
		m_vGlyphIndices.emplace_back(VertexBase + 3);
		m_vGlyphIndices.emplace_back(VertexBase + 2);

		size_t ByteCount{ GetUTF8CharacterByteCount(UTF8String[BufferAt]) };
		UUTF8_ID UTF8{};
		memcpy(UTF8.Chars, &UTF8String[BufferAt], ByteCount);
		BufferAt += ByteCount;

		size_t CharIndex{ (CharIndexMap.find(UTF8.ID) != CharIndexMap.end()) ? CharIndexMap.at(UTF8.ID) : 0 };
		PushGlyph(CharIndex, CursorX, CursorY);
	}
}

void CBMFontRenderer::RenderRegistered()
{
	UpdateStringCapacity();

	Render();
}

void CBMFontRenderer::Render()
{
	m_CBSpace->Use(EShaderType::VertexShader, m_CBSpaceSlot);

	m_FontTexture->Use();

	m_VSFont->Use();
	m_PSFont->Use();

	ID3D11SamplerState* Samplers[]{ m_CommonStates->PointClamp() };
	m_PtrDeviceContext->PSSetSamplers(0, 1, Samplers);

	m_PtrDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_PtrDeviceContext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &m_VertexBufferStride, &m_VertexBufferOffset);
	m_PtrDeviceContext->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	m_PtrDeviceContext->DrawIndexed(static_cast<UINT>(m_vGlyphIndices.size()), 0, 0);
}

bool CBMFontRenderer::UpdateStringCapacity()
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
