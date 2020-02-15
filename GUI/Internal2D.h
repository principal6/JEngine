#pragma once

#include "CommonTypes.h"
#include <vector>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;

class CInternal2D
{
public:
	CInternal2D(ID3D11Device* const Device, ID3D11DeviceContext* const DeviceContext);
	~CInternal2D();

public:
	void CreateColor(const SInt2& Size, const SFloat4& Color, float Roundness = 0);
	void CreateImage(const SInt2& Size, const SFloat4& TexCoordRange);

private:
	void CreateBuffers();

public:
	void UpdateBuffers();

public:
	void Destroy();

public:
	void Draw() const;

public:
	std::vector<SVertex>& GetVertices();

private:
	ID3D11Device*				m_PtrDevice{};
	ID3D11DeviceContext*		m_PtrDeviceContext{};

private:
	ID3D11Buffer*				m_VertexBuffer{};
	uint32_t					m_VertexBufferStride{ sizeof(SVertex) };
	uint32_t					m_VertexBufferOffset{};
	ID3D11Buffer*				m_IndexBuffer{};

private:
	std::vector<SVertex>		m_vVertices{};
	std::vector<uint32_t>		m_vIndices{};
};
