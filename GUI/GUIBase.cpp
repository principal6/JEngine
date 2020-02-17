#include "GUIBase.h"
#include "../Core/SharedHeader.h"
#include "../Core/Shader.h"
#include "../Core/ConstantBuffer.h"
#include "../DirectXTex/DirectXTex.h"

using std::string;
using std::wstring;
using std::make_unique;

CGUIBase::CGUIBase(ID3D11Device* const Device, ID3D11DeviceContext* const DeviceContext) :
	m_PtrDevice{ Device }, m_PtrDeviceContext{ DeviceContext }
{
	assert(m_PtrDevice);
	assert(m_PtrDeviceContext);
}

CGUIBase::~CGUIBase()
{
}

void CGUIBase::_Create(HWND hWnd, const std::string& BFNTFileName)
{
	m_hWnd = hWnd;

	m_CBSpace = make_unique<CConstantBuffer>(m_PtrDevice, m_PtrDeviceContext, &m_CBSpaceData, sizeof(m_CBSpaceData));
	m_CBSpace->Create();

	m_VS = make_unique<CShader>(m_PtrDevice, m_PtrDeviceContext);
	m_VS->Create(EShaderType::VertexShader, CShader::EVersion::_4_0, true, L"GUI\\VS.hlsl", "main", KInputLayout, ARRAYSIZE(KInputLayout));
	
	m_PS = make_unique<CShader>(m_PtrDevice, m_PtrDeviceContext);
	m_PS->Create(EShaderType::PixelShader, CShader::EVersion::_4_0, true, L"GUI\\PS.hlsl", "main");

	WINDOWINFO info{};
	GetWindowInfo(m_hWnd, &info);
	XMFLOAT2 WindowSize{
		(float)(info.rcClient.right - info.rcClient.left),
		(float)(info.rcClient.bottom - info.rcClient.top)
	};

	m_BFNTRenderer.Create(BFNTFileName, WindowSize);

	m_CaretBlinkTime = GetCaretBlinkTime();
}

void CGUIBase::_CreateTextureAtlas(const std::string& FileName)
{
	ID3D11Texture2D* Texture{};
	wstring _FileName{};
	{
		int WStringLen{ MultiByteToWideChar(CP_ACP, 0, FileName.c_str(), (int)FileName.size(), nullptr, 0) };
		wchar_t* WString{ new wchar_t[(size_t)WStringLen + 1] {} };
		MultiByteToWideChar(CP_ACP, 0, FileName.c_str(), (int)FileName.size(), WString, WStringLen);
		_FileName = WString;
		delete[] WString;
		WString = nullptr;
	}

	if (SUCCEEDED(CreateWICTextureFromFile(m_PtrDevice, _FileName.c_str(), (ID3D11Resource**)&Texture, m_Atlas.ReleaseAndGetAddressOf())))
	{
		Texture->GetDesc(&m_AtlasDesc);
		Texture->Release();
		Texture = nullptr;
	}
}

bool CGUIBase::_CreateButton(const std::string& Name, const SInt2& Size, CWidget* const ParentWidget)
{
	CWidget* NewButton{ new CButton(m_PtrDevice, m_PtrDeviceContext, &m_BFNTRenderer) };
	string FullName{ Name };
	if (ParentWidget) FullName = ParentWidget->GetName() + FullName;
	if (m_WidgetPool.Insert(FullName, NewButton))
	{
		((CButton*)NewButton)->Create(Name, Size, ParentWidget);

		return true;
	}
	return false;
}

bool CGUIBase::_CreateButtonPreset(const std::string& Name, const SInt2& Size, EButtonPreset ePreset, CWidget* const ParentWidget)
{
	CWidget* NewButton{ new CButton(m_PtrDevice, m_PtrDeviceContext, &m_BFNTRenderer) };
	string FullName{ Name };
	if (ParentWidget) FullName = ParentWidget->GetName() + FullName;
	if (m_WidgetPool.Insert(FullName, NewButton))
	{
		((CButton*)NewButton)->CreatePreset(Name, Size, ePreset, ParentWidget);
		return true;
	}
	return false;
}

bool CGUIBase::_CreateImage(const std::string& Name, const SInt2& Size, CWidget* const ParentWidget)
{
	return _CreateImage(Name, Size, SInt2(0, 0), ParentWidget);
}

bool CGUIBase::_CreateImage(const std::string& Name, const SInt2& Size, const SInt2& U0PixelCoord, CWidget* const ParentWidget)
{
	return _CreateImage(Name, Size, U0PixelCoord, Size, ParentWidget);
}

bool CGUIBase::_CreateImage(const std::string& Name, const SInt2& Size, const SInt2& U0PixelCoord, const SInt2& SizeInTexturePixelCoord, 
	CWidget* const ParentWidget)
{
	CWidget* NewImage{ new CImage(m_PtrDevice, m_PtrDeviceContext, &m_BFNTRenderer) };
	string FullName{ Name };
	if (ParentWidget) FullName = ParentWidget->GetName() + FullName;
	if (m_WidgetPool.Insert(FullName, NewImage))
	{
		float AtlasWidth{ (float)m_AtlasDesc.Width };
		float AtlasHeight{ (float)m_AtlasDesc.Height };

		float U0{ (float)U0PixelCoord.X / AtlasWidth };
		float V0{ (float)U0PixelCoord.Y / AtlasHeight };
		float U1{ U0 + (float)SizeInTexturePixelCoord.X / AtlasWidth };
		float V1{ V0 + (float)SizeInTexturePixelCoord.Y / AtlasHeight };

		((CImage*)NewImage)->Create(Name, Size, SFloat4(U0, V0, U1, V1), ParentWidget);
		return true;
	}
	return false;
}

bool CGUIBase::_CreateImageButton(const std::string& Name, const SInt2& Size, CWidget* const ParentWidget)
{
	CWidget* NewImageButton{ new CImageButton(m_PtrDevice, m_PtrDeviceContext, &m_BFNTRenderer) };
	string FullName{ Name };
	if (ParentWidget) FullName = ParentWidget->GetName() + FullName;
	if (m_WidgetPool.Insert(FullName, NewImageButton))
	{
		((CImageButton*)NewImageButton)->Create(Name, Size, ParentWidget);
		return true;
	}
	return false;
}

bool CGUIBase::_CreateWindowImage(const std::string& Name, const SInt2& Size, 
	const SInt2& U0PixelCoord, const SInt2& SizeInTexturePixelCoord, 
	const SInt2& TitleBarOffset, const SInt2& TitleBarSize, CWidget* const ParentWidget)
{
	CWidget* NewWindow{ new CWindow(m_PtrDevice, m_PtrDeviceContext, &m_BFNTRenderer) };
	string FullName{ Name };
	if (ParentWidget) FullName = ParentWidget->GetName() + FullName;
	if (m_WidgetPool.Insert(FullName, NewWindow))
	{
		float AtlasWidth{ (float)m_AtlasDesc.Width };
		float AtlasHeight{ (float)m_AtlasDesc.Height };

		float U0{ (float)U0PixelCoord.X / AtlasWidth };
		float V0{ (float)U0PixelCoord.Y / AtlasHeight };
		float U1{ U0 + (float)SizeInTexturePixelCoord.X / AtlasWidth };
		float V1{ V0 + (float)SizeInTexturePixelCoord.Y / AtlasHeight };

		((CWindow*)NewWindow)->CreateImage(Name, Size, SFloat4(U0, V0, U1, V1), ParentWidget);
		((CWindow*)NewWindow)->SetTitleBar(TitleBarOffset, TitleBarSize);

		return true;
	}
	return false;
}

bool CGUIBase::_CreateWindowPrimitive(const std::string& Name, const SInt2& Size, float Roundness, CWidget* const ParentWidget)
{
	CWidget* NewWindow{ new CWindow(m_PtrDevice, m_PtrDeviceContext, &m_BFNTRenderer) };
	string FullName{ Name };
	if (ParentWidget) FullName = ParentWidget->GetName() + FullName;
	if (m_WidgetPool.Insert(FullName, NewWindow))
	{
		((CWindow*)NewWindow)->CreatePrimitive(Name, Size, Roundness, ParentWidget);
		{
			assert(_CreateButtonPreset(KSysCloseID, SInt2(KDefaultTitleBarHeight, KDefaultTitleBarHeight), EButtonPreset::Close, NewWindow));
			CWidget* Widget{ GetWidget(KSysCloseID, NewWindow) };
			Widget->SetOffset(SInt2(Size.X - KDefaultTitleBarHeight, 0));
		}
		return true;
	}
	return false;
}

bool CGUIBase::_CreateText(const std::string& Name, const SInt2& Size, const std::string& Content, CWidget* const ParentWidget)
{
	CWidget* NewText{ new CText(m_PtrDevice, m_PtrDeviceContext, &m_BFNTRenderer) };
	string FullName{ Name };
	if (ParentWidget) FullName = ParentWidget->GetName() + FullName;
	if (m_WidgetPool.Insert(FullName, NewText))
	{
		((CText*)NewText)->Create(Name, Size, ParentWidget);
		((CText*)NewText)->SetCaption(Content);

		return true;
	}
	return false;
}

CWidget* CGUIBase::GetWidget(const std::string& Name, CWidget* const ParentWidget) const
{
	string FullName{ Name };
	if (ParentWidget) FullName = ParentWidget->GetName() + FullName;
	return m_WidgetPool.Get(FullName);
}

void CGUIBase::SetWidgetStateTexCoordRange(const std::string& FullName, EWidgetState eWidgetState,
	const SInt2& U0PixelCoord, const SInt2& SizeInTexturePixelCoord)
{
	if (m_WidgetPool.Exists(FullName))
	{
		auto Widget{ m_WidgetPool.Get(FullName) };

		float AtlasWidth{ (float)m_AtlasDesc.Width };
		float AtlasHeight{ (float)m_AtlasDesc.Height };

		float U0{ (float)U0PixelCoord.X / AtlasWidth };
		float V0{ (float)U0PixelCoord.Y / AtlasHeight };
		float U1{ U0 + (float)SizeInTexturePixelCoord.X / AtlasWidth };
		float V1{ V0 + (float)SizeInTexturePixelCoord.Y / AtlasHeight };

		Widget->SetStateTexCoordRange(eWidgetState, SFloat4(U0, V0, U1, V1));
	}
}

void CGUIBase::SetFocus(CWidget* const Widget)
{
	m_FocusedWidget = Widget;
}

bool CGUIBase::HasEvent() const
{
	return (m_EventQueue.size() ? true : false);
}

SEvent CGUIBase::GetEvent() const
{
	SEvent Front{ m_EventQueue.front() };
	m_EventQueue.pop_front();
	return Front;
}

bool CGUIBase::GenerateEvent(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	SEvent NewEvent{};
	switch (Msg)
	{
	case WM_LBUTTONDOWN:
		NewEvent.eEventType = EEventType::MouseDown;
		NewEvent.Extra = (uint32_t)EMouseEventType::LeftButton;
		m_bMouseDown = true;
		break;
	case WM_RBUTTONDOWN:
		NewEvent.eEventType = EEventType::MouseDown;
		NewEvent.Extra = (uint32_t)EMouseEventType::RightButton;
		m_bMouseDown = true;
		break;
	case WM_MBUTTONDOWN:
		NewEvent.eEventType = EEventType::MouseDown;
		NewEvent.Extra = (uint32_t)EMouseEventType::MiddleButton;
		m_bMouseDown = true;
		break;
	case WM_XBUTTONDOWN:
		NewEvent.eEventType = EEventType::MouseDown;
		NewEvent.Extra = (uint32_t)EMouseEventType::ExtraButton;
		m_bMouseDown = true;
		break;
	case WM_LBUTTONUP:
		NewEvent.eEventType = EEventType::MouseUp;
		NewEvent.Extra = (uint32_t)EMouseEventType::LeftButton;
		m_bMouseDown = false;
		break;
	case WM_RBUTTONUP:
		NewEvent.eEventType = EEventType::MouseUp;
		NewEvent.Extra = (uint32_t)EMouseEventType::RightButton;
		m_bMouseDown = false;
		break;
	case WM_MBUTTONUP:
		NewEvent.eEventType = EEventType::MouseUp;
		NewEvent.Extra = (uint32_t)EMouseEventType::MiddleButton;
		m_bMouseDown = false;
		break;
	case WM_XBUTTONUP:
		NewEvent.eEventType = EEventType::MouseUp;
		NewEvent.Extra = (uint32_t)EMouseEventType::ExtraButton;
		m_bMouseDown = false;
		break;
	case WM_MOUSEMOVE:
		NewEvent.eEventType = EEventType::MouseMove;
		break;
	case WM_MOUSEWHEEL:
	case WM_MOUSEHOVER:
	case WM_INPUT:
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		break;
	case WM_CHAR:
		NewEvent.eEventType = EEventType::KeyStroke;
		NewEvent.Extra = (uint32_t)wParam;
		break;
	default:
		break;
	}

	POINT MouseCursor{};
	GetCursorPos(&MouseCursor);
	ScreenToClient(m_hWnd, &MouseCursor);
	SInt2 MousePosition{ SInt2(MouseCursor.x, MouseCursor.y) };

	const auto& Names{ m_WidgetPool.GetNames() };
	for (const auto& Name : Names)
	{
		auto Widget{ m_WidgetPool.Get(Name) };
		bool bIsActive{ (Widget->IsChild(m_FocusedWidget) ? true : Widget == m_FocusedWidget) };
		Widget->UpdateState(MousePosition, NewEvent.eEventType, m_bMouseDown, m_LastMouseDownWidget, bIsActive);

		// These events need to be passed to the queue
		if (NewEvent.eEventType == EEventType::MouseUp ||
			NewEvent.eEventType == EEventType::MouseDown ||
			NewEvent.eEventType == EEventType::MouseMove ||
			NewEvent.eEventType == EEventType::Clicked)
		{
			if (Widget->IsMouseInside(MousePosition))
			{
				NewEvent.Widget = Widget;

				if (NewEvent.eEventType == EEventType::Clicked)
				{
					m_FocusedWidget = Widget;

					if (Widget->GetType() == EWidgetType::Window)
					{
						NewEvent.eEventType = EEventType::MouseUp; // @important: prevent corruption
					}
					if (Widget->GetName() == KSysCloseID)
					{
						CWidget* const Parent{ Widget->GetParent() };
						if (Parent && Parent->GetType() == EWidgetType::Window)
						{
							((CWindow*)Parent)->Close();
						}
					}
				}
			}
		}

		if (Widget->GetType() == EWidgetType::Window)
		{
			CWindow* Window{ (CWindow*)Widget };
			if (Window->IsCaptured())
			{
				if (NewEvent.eEventType == EEventType::MouseUp || NewEvent.eEventType == EEventType::Clicked) // @ EEventType::Clicked is impossible here?
				{
					Widget->Release();
				}
				else if (NewEvent.eEventType == EEventType::MouseMove)
				{
					const auto& CapturedMousePosition{ Widget->GetCapturedMousePosition() };
					const auto& CapturedOffset{ Widget->GetCapturedOffset() };
					auto DeltaMousePosition{ MousePosition - CapturedMousePosition };
					Widget->SetOffset(CapturedOffset + DeltaMousePosition);
				}
				NewEvent.Widget = Widget;
			}
			else if (Window->IsMouseOnTitleBar(MousePosition))
			{
				if (NewEvent.eEventType == EEventType::MouseDown)
				{
					Widget->Capture(MousePosition);
					NewEvent.Widget = Widget;
				}
			}
		}
	}

	if (NewEvent.Widget)
	{
		if (NewEvent.eEventType == EEventType::MouseDown) m_LastMouseDownWidget = NewEvent.Widget;
		m_EventQueue.emplace_back(NewEvent);
		return true;
	}

	if (NewEvent.eEventType == EEventType::MouseDown) m_LastMouseDownWidget = nullptr;
	return false;
}

void CGUIBase::Render() const
{
	WINDOWINFO info{};
	GetWindowInfo(m_hWnd, &info);
	XMFLOAT2 WindowSize{ 
		(float)(info.rcClient.right - info.rcClient.left),
		(float)(info.rcClient.bottom - info.rcClient.top)
	};
	m_CBSpaceData.Projection = XMMatrixOrthographicLH(WindowSize.x, WindowSize.y, 0, 1);

	if (m_Atlas)
	{
		m_PtrDeviceContext->PSSetShaderResources(KAtlasSlot, 1, m_Atlas.GetAddressOf());
	}

	XMFLOAT2 HalfWindowSize{ WindowSize.x * 0.5f, WindowSize.y * 0.5f };
	const auto& WidgetNames{ m_WidgetPool.GetNames() };
	for (const auto& WidgetName : WidgetNames)
	{
		const auto& Widget{ m_WidgetPool.Get(WidgetName) };
		EWidgetType eType{ Widget->GetType() };
		if (!Widget->ShouldDraw()) continue;
		if (eType == EWidgetType::Image && ((CImage*)Widget)->GetSource())
		{
			auto Src{ ((CImage*)Widget)->GetSource() };
			m_PtrDeviceContext->PSSetShaderResources(KImageSlot, 1, &Src);
		}

		const auto& Position{ Widget->GetPosition() };
		const auto& Offset{ Widget->GetOffset() };
		auto Parent{ Widget->GetParent() };
		const auto& ParentPosition{ (Parent) ? Parent->GetPosition() : SInt2() };
		const auto& ParentOffset{ (Parent) ? Parent->GetOffset() : SInt2() };
		
		m_VS->Use();
		m_PS->Use();
		m_CBSpaceData.Offset.x = -HalfWindowSize.x + (float)ParentPosition.X + (float)ParentOffset.X + (float)Position.X + (float)Offset.X;
		m_CBSpaceData.Offset.y = +HalfWindowSize.y - (float)ParentPosition.Y - (float)ParentOffset.Y - (float)Position.Y - (float)Offset.Y;
		m_CBSpace->Update();
		m_CBSpace->Use(EShaderType::VertexShader, 0);

		Widget->Draw();
	}
}
