#include "Internal2D.h"
#include "../Core/SharedHeader.h"

using std::max;
using std::min;

#define SAFE_RELEASE(a) if (a) { a->Release(); a = nullptr; }

CInternal2D::CInternal2D(ID3D11Device* const Device, ID3D11DeviceContext* const DeviceContext) :
	m_PtrDevice{ Device }, m_PtrDeviceContext{ DeviceContext }
{
	assert(m_PtrDevice);
	assert(m_PtrDeviceContext);
}

CInternal2D::~CInternal2D()
{
	Destroy();
}

void CInternal2D::CreateColor(const SInt2& Size, const SFloat4& Color, float Roundness)
{
	static constexpr size_t KDetail{ 16 };
	XMFLOAT2 _Size{ (float)Size.X, (float)Size.Y };
	Roundness = min(max(Roundness, 0.0f), 1.0f);

	if (Roundness > 0)
	{
		float RadiusMax{ min(_Size.x * 0.5f, _Size.y * 0.5f) };
		float Radius{ Roundness * RadiusMax };

		XMFLOAT2 InnerSize{ _Size.x - Radius * 2, _Size.y - Radius * 2 };

		// Inner top-bottom
		m_vVertices.insert(
			m_vVertices.end(),
			{
				SVertex(SFloat4(Radius					, 0			, 0, 1), Color),
				SVertex(SFloat4(Radius + InnerSize.x	, 0			, 0, 1), Color),
				SVertex(SFloat4(Radius					, -_Size.y	, 0, 1), Color),
				SVertex(SFloat4(Radius + InnerSize.x	, -_Size.y	, 0, 1), Color)
			}
		);
		m_vIndices.insert(
			m_vIndices.end(),
			{ 4 * 0 + 0, 4 * 0 + 1, 4 * 0 + 2, 4 * 0 + 1, 4 * 0 + 3, 4 * 0 + 2 }
		);

		// Inner left
		m_vVertices.insert(
			m_vVertices.end(),
			{
				SVertex(SFloat4(0			, -Radius				, 0, 1), Color),
				SVertex(SFloat4(0 + Radius	, -Radius				, 0, 1), Color),
				SVertex(SFloat4(0			, -Radius - InnerSize.y	, 0, 1), Color),
				SVertex(SFloat4(0 + Radius	, -Radius - InnerSize.y	, 0, 1), Color)
			}
		);
		m_vIndices.insert(
			m_vIndices.end(),
			{ 4 * 1 + 0, 4 * 1 + 1, 4 * 1 + 2, 4 * 1 + 1, 4 * 1 + 3, 4 * 1 + 2 }
		);

		// Inner right
		m_vVertices.insert(
			m_vVertices.end(),
			{
				SVertex(SFloat4(Radius + InnerSize.x			, -Radius				, 0, 1), Color),
				SVertex(SFloat4(Radius + InnerSize.x + Radius	, -Radius				, 0, 1), Color),
				SVertex(SFloat4(Radius + InnerSize.x			, -Radius - InnerSize.y	, 0, 1), Color),
				SVertex(SFloat4(Radius + InnerSize.x + Radius	, -Radius - InnerSize.y	, 0, 1), Color)
			}
		);
		m_vIndices.insert(
			m_vIndices.end(),
			{ 4 * 2 + 0, 4 * 2 + 1, 4 * 2 + 2, 4 * 2 + 1, 4 * 2 + 3, 4 * 2 + 2 }
		);

		// Corner left-up
		{
			uint32_t IndexOffset{ (uint32_t)(4 * 3 + KDetail * 3 * 0) };
			float ThetaUnit{ XM_PIDIV2 / (float)KDetail };
			for (size_t i = 0; i < KDetail; ++i)
			{
				float Theta0{ ThetaUnit * (i + 0) };
				float Theta1{ ThetaUnit * (i + 1) };
				m_vVertices.emplace_back(SVertex(SFloat4(Radius - cos(Theta1) * Radius	, -Radius + sin(Theta1) * Radius, 0, 1), Color));
				m_vVertices.emplace_back(SVertex(SFloat4(Radius							, -Radius						, 0, 1), Color));
				m_vVertices.emplace_back(SVertex(SFloat4(Radius - cos(Theta0) * Radius	, -Radius + sin(Theta0) * Radius, 0, 1), Color));

				m_vIndices.emplace_back((uint32_t)(IndexOffset + i * 3 + 0));
				m_vIndices.emplace_back((uint32_t)(IndexOffset + i * 3 + 1));
				m_vIndices.emplace_back((uint32_t)(IndexOffset + i * 3 + 2));
			}
		}

		// Corner right-up
		{
			uint32_t IndexOffset{ (uint32_t)(4 * 3 + KDetail * 3 * 1) };
			float ThetaUnit{ XM_PIDIV2 / KDetail };
			for (size_t i = 0; i < (size_t)KDetail; ++i)
			{
				float Theta0{ ThetaUnit * (i + 0) };
				float Theta1{ ThetaUnit * (i + 1) };
				m_vVertices.emplace_back(SVertex(SFloat4(Radius + InnerSize.x + cos(Theta1) * Radius, -Radius + sin(Theta1) * Radius, 0, 1), Color));
				m_vVertices.emplace_back(SVertex(SFloat4(Radius + InnerSize.x + cos(Theta0) * Radius, -Radius + sin(Theta0) * Radius, 0, 1), Color));
				m_vVertices.emplace_back(SVertex(SFloat4(Radius	+ InnerSize.x 						, -Radius						, 0, 1), Color));

				m_vIndices.emplace_back((uint32_t)(IndexOffset + i * 3 + 0));
				m_vIndices.emplace_back((uint32_t)(IndexOffset + i * 3 + 1));
				m_vIndices.emplace_back((uint32_t)(IndexOffset + i * 3 + 2));
			}
		}

		// Corner left-down
		{
			uint32_t IndexOffset{ (uint32_t)(4 * 3 + KDetail * 3 * 2) };
			float ThetaUnit{ XM_PIDIV2 / (float)KDetail };
			for (size_t i = 0; i < KDetail; ++i)
			{
				float Theta0{ ThetaUnit * (i + 0) };
				float Theta1{ ThetaUnit * (i + 1) };
				m_vVertices.emplace_back(SVertex(SFloat4(Radius - cos(Theta0) * Radius	, -Radius - InnerSize.y - sin(Theta0) * Radius	, 0, 1), Color));
				m_vVertices.emplace_back(SVertex(SFloat4(Radius							, -Radius - InnerSize.y							, 0, 1), Color));
				m_vVertices.emplace_back(SVertex(SFloat4(Radius - cos(Theta1) * Radius	, -Radius - InnerSize.y - sin(Theta1) * Radius	, 0, 1), Color));

				m_vIndices.emplace_back((uint32_t)(IndexOffset + i * 3 + 0));
				m_vIndices.emplace_back((uint32_t)(IndexOffset + i * 3 + 1));
				m_vIndices.emplace_back((uint32_t)(IndexOffset + i * 3 + 2));
			}
		}

		// Corner right-down
		{
			uint32_t IndexOffset{ (uint32_t)(4 * 3 + KDetail * 3 * 3) };
			float ThetaUnit{ XM_PIDIV2 / (float)KDetail };
			for (size_t i = 0; i < KDetail; ++i)
			{
				float Theta0{ ThetaUnit * (i + 0) };
				float Theta1{ ThetaUnit * (i + 1) };
				m_vVertices.emplace_back(SVertex(SFloat4(Radius + InnerSize.x						, -Radius - InnerSize.y							, 0, 1), Color));
				m_vVertices.emplace_back(SVertex(SFloat4(Radius + InnerSize.x + cos(Theta0) * Radius, -Radius - InnerSize.y - sin(Theta0) * Radius	, 0, 1), Color));
				m_vVertices.emplace_back(SVertex(SFloat4(Radius + InnerSize.x + cos(Theta1) * Radius, -Radius - InnerSize.y - sin(Theta1) * Radius	, 0, 1), Color));

				m_vIndices.emplace_back((uint32_t)(IndexOffset + i * 3 + 0));
				m_vIndices.emplace_back((uint32_t)(IndexOffset + i * 3 + 1));
				m_vIndices.emplace_back((uint32_t)(IndexOffset + i * 3 + 2));
			}
		}
	}
	else
	{
		m_vVertices.assign(
			{
				SVertex(SFloat4(0		, 0			, 0, 1), Color),
				SVertex(SFloat4(_Size.x	, 0			, 0, 1), Color),
				SVertex(SFloat4(0		, -_Size.y	, 0, 1), Color),
				SVertex(SFloat4(_Size.x	, -_Size.y	, 0, 1), Color)
			});
		m_vIndices.assign({ 0, 1, 2, 1, 3, 2 });
	}

	CreateBuffers();
}

void CInternal2D::CreateImage(const SInt2& Size, const SFloat4& TexCoordRange)
{
	XMFLOAT2 _Size{ (float)Size.X, (float)Size.Y };
	m_vVertices.assign(
		{
			SVertex(SFloat4(0		, 0			, 0, 1), SFloat4(TexCoordRange.X, TexCoordRange.Y, 0, -1)),
			SVertex(SFloat4(_Size.x	, 0			, 0, 1), SFloat4(TexCoordRange.Z, TexCoordRange.Y, 0, -1)),
			SVertex(SFloat4(0		, -_Size.y	, 0, 1), SFloat4(TexCoordRange.X, TexCoordRange.W, 0, -1)),
			SVertex(SFloat4(_Size.x	, -_Size.y	, 0, 1), SFloat4(TexCoordRange.Z, TexCoordRange.W, 0, -1))
		});
	m_vIndices.assign({ 0, 1, 2, 1, 3, 2 });

	CreateBuffers();
}

void CInternal2D::CreateBuffers()
{
	// Vertex buffer
	{
		D3D11_BUFFER_DESC BufferDesc{};
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		BufferDesc.ByteWidth = static_cast<UINT>(sizeof(SVertex) * m_vVertices.size());
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		BufferDesc.MiscFlags = 0;
		BufferDesc.StructureByteStride = 0;
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;

		D3D11_SUBRESOURCE_DATA SubresourceData{};
		SubresourceData.pSysMem = &m_vVertices[0];

		SAFE_RELEASE(m_VertexBuffer);
		m_PtrDevice->CreateBuffer(&BufferDesc, &SubresourceData, &m_VertexBuffer);
	}

	// Index buffer
	{
		D3D11_BUFFER_DESC BufferDesc{};
		BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		BufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * m_vIndices.size());
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		BufferDesc.MiscFlags = 0;
		BufferDesc.StructureByteStride = 0;
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;

		D3D11_SUBRESOURCE_DATA SubresourceData{};
		SubresourceData.pSysMem = &m_vIndices[0];

		SAFE_RELEASE(m_IndexBuffer);
		m_PtrDevice->CreateBuffer(&BufferDesc, &SubresourceData, &m_IndexBuffer);
	}
}

void CInternal2D::UpdateBuffers()
{
	D3D11_MAPPED_SUBRESOURCE MappedSubresource{};

	// Vertex buffer
	if (SUCCEEDED(m_PtrDeviceContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource)))
	{
		memcpy(MappedSubresource.pData, &m_vVertices[0], sizeof(SVertex) * m_vVertices.size());
		m_PtrDeviceContext->Unmap(m_VertexBuffer, 0);
	}

	// Index buffer
	if (SUCCEEDED(m_PtrDeviceContext->Map(m_IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource)))
	{
		memcpy(MappedSubresource.pData, &m_vIndices[0], sizeof(uint32_t) * m_vIndices.size());
		m_PtrDeviceContext->Unmap(m_IndexBuffer, 0);
	}
}

void CInternal2D::Destroy()
{
	SAFE_RELEASE(m_VertexBuffer);
	SAFE_RELEASE(m_IndexBuffer);
}

void CInternal2D::Draw() const
{
	m_PtrDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_PtrDeviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &m_VertexBufferStride, &m_VertexBufferOffset);
	m_PtrDeviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	m_PtrDeviceContext->DrawIndexed(static_cast<UINT>(m_vIndices.size()), 0, 0);
}

std::vector<SVertex>& CInternal2D::GetVertices()
{
	return m_vVertices;
}
