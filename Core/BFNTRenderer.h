#pragma once

#include "SharedHeader.h"
#include "BFNTTypes.h"

class CBFNTLoader;
class CTexture;
class CShader;
class CConstantBuffer;

class CBFNTRenderer
{
public:
	struct alignas(16) SVertex
	{
		SVertex() {};
		SVertex(const XMFLOAT2& _Position, const XMFLOAT2& _TexCoord) : Position{ _Position }, TexCoord{ _TexCoord } {}
		SVertex(float X, float Y, float U, float V) : Position{ X, Y }, TexCoord{ U, V } {}

		XMFLOAT2	Position{};
		XMFLOAT2	TexCoord{};
	};

	struct SCBSpaceData
	{
		XMMATRIX	ProjectionMatrix{};
		XMFLOAT4	Position{ 0, 0, 0, 1 };
	};

public:
	CBFNTRenderer(ID3D11Device* const PtrDevice, ID3D11DeviceContext* const PtrDeviceContext);
	CBFNTRenderer(const CBFNTRenderer& BFNTRenderer) = delete;
	~CBFNTRenderer();

public:
	void Create(const std::string& BFNT_FileName, const DirectX::XMFLOAT2& WindowSize);
	void Create(const CBFNTRenderer* const FontRenderer);

private:
	void CreateBuffers();
	void UpdateBuffers();
	void ReleaseResources();

public:
	void SetVSConstantBufferSlot(UINT Slot);
	void SetPSConstantBufferSlot(UINT Slot);

public:
	const SBFNTData& GetData() const;
	uint32_t CalculateStringWidth(const char* UTF8String);
	uint32_t CalculateCharacterAtFromOffsetX(const char* UTF8String, int32_t OffsetX);

public:
	void RenderString(const char* UTF8String, const DirectX::XMFLOAT2& Position, const DirectX::XMFLOAT4& Color);

private:
	void PushGlyph(size_t BufferCharIndex, size_t GlyphIndex, int32_t& CursorX, int32_t CursorY);
	void Render();
	bool UpdateStringCapacity();
	
private:
	static constexpr D3D11_INPUT_ELEMENT_DESC KInputLayout[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	static constexpr size_t			KInitialStringCapacity{ 64 };
	static constexpr size_t			KMaxStringCapacity{ 2048 };
	static constexpr UINT			KDefaultCBSpaceSlot{ KVSSharedCBCount };
	static constexpr UINT			KDefaultCBColorSlot{ KPSSharedCBCount };

private:
	ID3D11Device* const				m_PtrDevice{};
	ID3D11DeviceContext* const		m_PtrDeviceContext{};

// These variables could be owned or referenced
private:
	bool							m_bHasOwnData{ false };
	CBFNTLoader*					m_BFNTLoader{};
	CTexture*						m_FontTexture{};
	CShader*						m_VSFont{};
	CShader*						m_PSFont{};
	SCBSpaceData*					m_CBSpaceData{};
	CConstantBuffer*				m_CBSpace{};
	UINT							m_CBSpaceSlot{ KDefaultCBSpaceSlot };
	DirectX::XMFLOAT4*				m_CBColorData{};
	CConstantBuffer*				m_CBColor{};
	UINT							m_CBColorSlot{ KDefaultCBColorSlot };
	CommonStates*					m_CommonStates{};

private:
	DirectX::XMFLOAT2				m_WindowSize{};

private:
	UINT							m_VertexBufferStride{ sizeof(SVertex) };
	UINT							m_VertexBufferOffset{};
	ComPtr<ID3D11Buffer>			m_VertexBuffer{};
	ComPtr<ID3D11Buffer>			m_IndexBuffer{};

private:
	size_t							m_PrevRenderingHash{};
	size_t							m_StringCapacity{ KInitialStringCapacity };
	std::vector<SVertex>			m_vGlyphVertices{};
	std::vector<UINT>				m_vGlyphIndices{};

private:
	size_t							m_PrevStringWidthHash{};
	uint32_t						m_PrevStringWidth{};
};
