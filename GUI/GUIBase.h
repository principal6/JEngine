#pragma once

#include <vector>
#include <string>
#include <d3d11.h>
#include <memory>
#include <deque>
#include "Widget.h"
#include "DynamicPoolDX11.h"
#include "../DirectXTK/DirectXTK.h"
#include "../Core/DynamicPool.h"
#include "../Core/BFNTRenderer.h"

class CShader;
class CConstantBuffer;

struct SEvent
{
	EEventType	eEventType{};
	CWidget*	Widget{};
	uint32_t	Extra{};
};

class CGUIBase
{
protected:
	struct SCBSpaceData
	{
		DirectX::XMMATRIX Projection{};
		DirectX::XMFLOAT4 Offset{};
	};

public:
	CGUIBase(ID3D11Device* const Device, ID3D11DeviceContext* const DeviceContext);
	virtual ~CGUIBase();

protected:
	void _Create(HWND hWnd, const std::string& BFNTFileName);

// Texture
protected:
	void _CreateTextureAtlas(const std::string& FileName);

protected:
	bool _CreateButton(const std::string& Name, const SInt2& Size, CWidget* const ParentWidget = nullptr);
	bool _CreateButtonPreset(const std::string& Name, const SInt2& Size, EButtonPreset ePreset, CWidget* const ParentWidget = nullptr);
	bool _CreateImage(const std::string& Name, const SInt2& Size, CWidget* const ParentWidget = nullptr);
	bool _CreateImage(const std::string& Name, const SInt2& Size, const SInt2& U0PixelCoord, CWidget* const ParentWidget = nullptr);
	bool _CreateImage(const std::string& Name, const SInt2& Size, const SInt2& U0PixelCoord, 
		const SInt2& SizeInTexturePixelCoord, CWidget* const ParentWidget = nullptr);
	bool _CreateImageButton(const std::string& Name, const SInt2& Size, CWidget* const ParentWidget = nullptr);
	bool _CreateWindowImage(const std::string& Name, const SInt2& Size,
		const SInt2& U0PixelCoord, const SInt2& SizeInTexturePixelCoord,
		const SInt2& TitleBarOffset, const SInt2& TitleBarSize, CWidget* const ParentWidget = nullptr);
	bool _CreateWindowPrimitive(const std::string& Name, const SInt2& Size, float Roundness, CWidget* const ParentWidget = nullptr);
	bool _CreateText(const std::string& Name, const SInt2& Size, const std::string& Content, CWidget* const ParentWidget = nullptr);

public:
	CWidget* GetWidget(const std::string& Name, CWidget* const ParentWidget = nullptr) const;

protected:
	void SetWidgetStateTexCoordRange(const std::string& FullName, EWidgetState eWidgetState,
		const SInt2& U0PixelCoord, const SInt2& SizeInTexturePixelCoord);

public:
	void SetFocus(CWidget* const Widget);

public:
	bool HasEvent() const;
	SEvent GetEvent() const;

public:
	bool GenerateEvent(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

public:
	void Render() const;

// Constants
public:
	static constexpr const char* KSysCloseID{ "_sys_close" };

// Constants
protected:
	static constexpr D3D11_INPUT_ELEMENT_DESC KInputLayout[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT	, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLORTEX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT	, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	static constexpr uint32_t KImageSlot{ 90 };
	static constexpr uint32_t KAtlasSlot{ 91 };

// Device & device context & window handle
protected:
	ID3D11Device*						m_PtrDevice{};
	ID3D11DeviceContext*				m_PtrDeviceContext{};
	HWND								m_hWnd{};

// Texture
protected:
	ComPtr<ID3D11ShaderResourceView>	m_Atlas{};
	D3D11_TEXTURE2D_DESC				m_AtlasDesc{};

// Shaders
protected:
	std::unique_ptr<CShader>			m_VS{};
	std::unique_ptr<CShader>			m_PS{};
	std::unique_ptr<CConstantBuffer>	m_CBSpace{};
	mutable SCBSpaceData				m_CBSpaceData{};

protected:
	CDynamicPool<CWidget>				m_WidgetPool{};

protected:
	CBFNTRenderer						m_BFNTRenderer{ m_PtrDevice, m_PtrDeviceContext };

protected:
	mutable std::deque<SEvent>			m_EventQueue{};
	CWidget*							m_LastMouseDownWidget{};
	CWidget*							m_FocusedWidget{};
	mutable bool						m_bMouseDown{ false };

protected:
	uint32_t							m_CaretBlinkTime{};
};
