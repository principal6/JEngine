#pragma once

#include "SharedHeader.h"

class CBMFont;
class CTexture;
class CShader;
class CConstantBuffer;

class CBMFontRenderer
{
public:
	struct alignas(16) SVertex
	{
		SVertex() {}
		SVertex(const XMFLOAT2& _Position, const XMFLOAT2& _TexCoord) :
			Position{ _Position }, TexCoord{ _TexCoord } {}

		XMFLOAT2	Position{};
		XMFLOAT2	TexCoord{};
		XMFLOAT4	Color{};
	};

	struct SGlyph
	{
		SVertex		Vertices[4]{};
	};

	struct SCBSpaceData
	{
		XMMATRIX	ProjectionMatrix{};
		XMFLOAT4	Position{};
	};

public:
	CBMFontRenderer(ID3D11Device* const PtrDevice, ID3D11DeviceContext* const PtrDeviceContext);
	CBMFontRenderer(const CBMFontRenderer& FontRenderer) = delete;
	~CBMFontRenderer();

public:
	void Create(const std::string& FNT_FileName, const DirectX::XMFLOAT2& WindowSize);
	void Create(const CBMFontRenderer* const FontRenderer);

private:
	void CreateBuffers();
	void UpdateBuffers();
	void ReleaseResources();

public:
	void SetVSConstantBufferSlot(UINT Slot);
	void SetFontColor(const DirectX::XMFLOAT4& Color);

public:
	// # Instant rendering function #0
	// Updates glyphs every time it's called (for mutable strings)
	// If the string is constant, call RenderConstantText() for better performance
	void RenderMutableString(const char* UTF8String, const DirectX::XMFLOAT2& Position);

	// # Instant rendering function #1
	// Updates glyphs only the first time the UTF8String is assigned (or re-assigned) (for constant strings)
	// If the string is mutable, call RenderMutableString()
	// If the string is constant, RenderConstantText() is better choice for performance than RenderMutableString()
	void RenderConstantString(const char* UTF8String, const DirectX::XMFLOAT2& Position);

public:
	// # Registration-based rendering function (step A)
	// Clears all the registered string and initialize necessary values
	void ClearRegistered();

	// # Registration-based rendering function (step B)
	// Register string in the internal stack, in order to render them later at once
	void RegisterString(const char* UTF8String, const DirectX::XMFLOAT2& Position, const DirectX::XMFLOAT4& Color);
	
	// # Registration-based rendering function (step C)
	// Render all the registered string and flush them
	void RenderRegistered();

private:
	CBMFontRenderer::SGlyph GetGlyph(size_t CharIndex, int32_t& CursorX, int32_t CursorY);
	void Render();
	
private:
	static constexpr D3D11_INPUT_ELEMENT_DESC KInputLayout[]
	{
		{ "POSITION"	, 0, DXGI_FORMAT_R32G32_FLOAT		, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD"	, 0, DXGI_FORMAT_R32G32_FLOAT		, 0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR"		, 0, DXGI_FORMAT_R32G32B32A32_FLOAT	, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	static constexpr size_t KInitialStringCapacity{ 128 };
	static constexpr UINT KDefaultCBSpaceSlot{ KVSSharedCBCount };

private:
	ID3D11Device* const				m_PtrDevice{};
	ID3D11DeviceContext* const		m_PtrDeviceContext{};

// These variables could be owned or referenced
private:
	bool							m_bHasOwnData{ false };
	CBMFont*						m_BMFont{};
	CTexture*						m_FontTexture{};
	CShader*						m_VSFont{};
	CShader*						m_PSFont{};
	CConstantBuffer*				m_CBSpace{};
	UINT							m_CBSpaceSlot{ KDefaultCBSpaceSlot };
	SCBSpaceData					m_CBSpaceData{};
	CommonStates*					m_CommonStates{};

private:
	UINT							m_VertexBufferStride{ sizeof(SVertex) };
	UINT							m_VertexBufferOffset{};
	ComPtr<ID3D11Buffer>			m_VertexBuffer{};
	ComPtr<ID3D11Buffer>			m_IndexBuffer{};

private:
	DirectX::XMFLOAT2				m_WindowSize{};
	DirectX::XMFLOAT2				m_TextureSizeDenominator{ 1.0f, 1.0f };
	DirectX::XMFLOAT4				m_FontColor{ 1, 1, 1, 1 };

private:
	const char*						m_PtrConstantUTF8String{};
	size_t							m_StringCapacity{ KInitialStringCapacity };
	std::vector<SGlyph>				m_vGlyphs{};
	std::vector<UINT>				m_vGlyphIndices{};
};
