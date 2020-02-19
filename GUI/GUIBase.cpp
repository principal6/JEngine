#include "GUIBase.h"
#include "../Core/SharedHeader.h"
#include "../Core/Shader.h"
#include "../Core/ConstantBuffer.h"
#include "../Core/UTF8.h"
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

	m_CaretBlinkTime = ::GetCaretBlinkTime();

	m_WidgetCtorData = SWidgetCtorData(m_PtrDevice, m_PtrDeviceContext, &m_BFNTRenderer, m_VS.get(), m_PS.get(), m_CBSpace.get(), &m_CBSpaceData, &m_WindowSize);

	D3D11_RASTERIZER_DESC RasterizerDesc;
	ZeroMemory(&RasterizerDesc, sizeof(RasterizerDesc));
	RasterizerDesc.FillMode = D3D11_FILL_SOLID;
	RasterizerDesc.CullMode = D3D11_CULL_NONE;
	RasterizerDesc.ScissorEnable = true;
	RasterizerDesc.DepthClipEnable = true;
	m_PtrDevice->CreateRasterizerState(&RasterizerDesc, m_RS.ReleaseAndGetAddressOf());
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
	CWidget* NewButton{ new CButton(m_WidgetCtorData) };
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
	CWidget* NewButton{ new CButton(m_WidgetCtorData) };
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
	CWidget* NewImage{ new CImage(m_WidgetCtorData) };
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
	CWidget* NewImageButton{ new CImageButton(m_WidgetCtorData) };
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
	CWidget* NewWindow{ new CWindow(m_WidgetCtorData) };
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
	CWidget* NewWindow{ new CWindow(m_WidgetCtorData) };
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
	CWidget* NewText{ new CText(m_WidgetCtorData) };
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

bool CGUIBase::_CreateTextEdit(const std::string& Name, const SInt2& Size, CWidget* const ParentWidget)
{
	CWidget* NewTextEdit{ new CTextEdit(m_WidgetCtorData) };
	string FullName{ Name };
	if (ParentWidget) FullName = ParentWidget->GetName() + FullName;
	if (m_WidgetPool.Insert(FullName, NewTextEdit))
	{
		((CTextEdit*)NewTextEdit)->Create(Name, Size, ParentWidget);
		((CTextEdit*)NewTextEdit)->m_PtrCaretBlinkTime = &m_CaretBlinkTime;
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

uint32_t CGUIBase::GetCaretBlinkTime() const
{
	return m_CaretBlinkTime;
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
		NewEvent.eEventType = EEventType::KeyDown;
		NewEvent.Extra = (uint32_t)wParam;
		break;
	case WM_CHAR:
		NewEvent.eEventType = EEventType::KeyStroke;
		NewEvent.Extra = (uint32_t)wParam;
		break;
	case WM_IME_COMPOSITION:
		if (lParam & GCS_COMPSTR)
		{
			HIMC hIMC{ ImmGetContext(hWnd) };
			LONG len{ ImmGetCompositionString(hIMC, GCS_COMPSTR, NULL, 0) };
			TCHAR tstr[3]{};
			ImmGetCompositionString(hIMC, GCS_COMPSTR, tstr, len);
			wchar_t wChar{};
			MultiByteToWideChar(CP_ACP, 0, tstr, (int)strlen(tstr), &wChar, 1);
			ImmReleaseContext(hWnd, hIMC);
			NewEvent.eEventType = EEventType::IMEComposition;
			NewEvent.Extra = wChar;

			m_bIsIMECompositing = true;
		}
		break;
	case WM_IME_CHAR:
	{
		TCHAR tstr[3]{};
		wchar_t wChar{};
		NewEvent.eEventType = EEventType::KeyStroke;
		tstr[0] = HIBYTE(LOWORD(wParam));
		tstr[1] = LOBYTE(LOWORD(wParam));
		MultiByteToWideChar(CP_ACP, 0, tstr, (int)strlen(tstr), &wChar, 1);
		NewEvent.Extra = wChar;

		m_bIsIMECompositing = false;
		break;
	}
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
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
		Widget->UpdateState(MousePosition, NewEvent.eEventType, m_bMouseDown, m_LastMouseDownWidget, m_FocusedWidget);

		// TextEdit key input
		if (NewEvent.eEventType == EEventType::KeyStroke || 
			NewEvent.eEventType == EEventType::KeyDown ||
			NewEvent.eEventType == EEventType::IMEComposition)
		{
			if (Widget->GetType() == EWidgetType::TextEdit && Widget->IsFocused())
			{
				CTextEdit* const TextEdit{ (CTextEdit*)Widget };
				if (NewEvent.eEventType == EEventType::IMEComposition)
				{
					if (!m_bIsIMECompositingPrev && m_bIsIMECompositing)
					{
						wchar_t wChar{ (wchar_t)NewEvent.Extra };
						UUTF8_ID utf8{ ConvertToUTF8(wChar) };
						TextEdit->InsertChar(utf8.Chars);
					}
					else if (m_bIsIMECompositingPrev && m_bIsIMECompositing)
					{
						TextEdit->DeletePreChar();
						wchar_t wChar{ (wchar_t)NewEvent.Extra };
						UUTF8_ID utf8{ ConvertToUTF8(wChar) };
						TextEdit->InsertChar(utf8.Chars);
					}
				}
				if (NewEvent.eEventType == EEventType::KeyStroke)
				{
					if (m_bIsIMECompositingPrev)
					{
						TextEdit->DeletePreChar();
					}

					wchar_t wChar{ (wchar_t)NewEvent.Extra };
					if (wChar == 0x03) // ctrl + c
					{
						TextEdit->CopyToClipboard();
					}
					else if (wChar == 0x08) // backspace
					{
						TextEdit->DeletePreChar();
					}
					else if (wChar == 0x16) // ctrl + v
					{
						TextEdit->PasteFromClipboard();
					}
					else if (wChar == 0x18) // ctrl + x
					{
						TextEdit->CutToClipboard();
					}
					else if (wChar >= (wchar_t)0x20)
					{
						UUTF8_ID utf8{ ConvertToUTF8(wChar) };
						TextEdit->InsertChar(utf8.Chars);
					}
				}
				else if (NewEvent.eEventType == EEventType::KeyDown)
				{
					if (NewEvent.Extra == VK_RIGHT)
					{
						if (GetAsyncKeyState(VK_SHIFT) < 0)
						{
							TextEdit->Select(CTextEdit::EDirection::Right);
						}
						else
						{
							TextEdit->MoveCaret(CTextEdit::EDirection::Right);
						}
					}
					else if (NewEvent.Extra == VK_LEFT)
					{
						if (GetAsyncKeyState(VK_SHIFT) < 0)
						{
							TextEdit->Select(CTextEdit::EDirection::Left);
						}
						else
						{
							TextEdit->MoveCaret(CTextEdit::EDirection::Left);
						}
					}
					else if (NewEvent.Extra == VK_HOME)
					{
						if (GetAsyncKeyState(VK_SHIFT) < 0)
						{
							TextEdit->Select(CTextEdit::EDirection::Home);
						}
						else
						{
							TextEdit->MoveCaret(CTextEdit::EDirection::Home);
						}
						
					}
					else if (NewEvent.Extra == VK_END)
					{
						if (GetAsyncKeyState(VK_SHIFT) < 0)
						{
							TextEdit->Select(CTextEdit::EDirection::End);
						}
						else
						{
							TextEdit->MoveCaret(CTextEdit::EDirection::End);
						}
					}
					else if (NewEvent.Extra == VK_DELETE)
					{
						TextEdit->DeletePostChar();
					}
				}
			}
		}

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

			// TextEdit mouse input
			if (Widget->GetType() == EWidgetType::TextEdit)
			{
				CTextEdit* const TextEdit{ (CTextEdit*)Widget };
				if (NewEvent.eEventType == EEventType::MouseDown)
				{
					if (Widget->IsMouseInside(MousePosition))
					{
						NewEvent.Widget = Widget;

						if (GetAsyncKeyState(VK_SHIFT) < 0)
						{
							TextEdit->Select(MousePosition);
						}
						else
						{
							TextEdit->MoveCaret(MousePosition);
						}
					}
				}
				if (NewEvent.eEventType == EEventType::MouseMove)
				{
					if (m_bMouseDown && Widget->IsFocused())
					{
						NewEvent.Widget = Widget;

						TextEdit->Select(MousePosition);
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

	m_bIsIMECompositingPrev = m_bIsIMECompositing; // @important

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
	m_PtrDeviceContext->RSSetState(m_RS.Get());

	WINDOWINFO info{};
	GetWindowInfo(m_hWnd, &info);
	m_WindowSize = SFloat2(
		(float)(info.rcClient.right - info.rcClient.left),
		(float)(info.rcClient.bottom - info.rcClient.top)
	);
	m_CBSpaceData.Projection = XMMatrixOrthographicLH(m_WindowSize.X, m_WindowSize.Y, 0, 1);

	if (m_Atlas)
	{
		m_PtrDeviceContext->PSSetShaderResources(KAtlasSlot, 1, m_Atlas.GetAddressOf());
	}

	XMFLOAT2 HalfWindowSize{ m_WindowSize.X * 0.5f, m_WindowSize.Y * 0.5f };
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

		Widget->Draw();
	}
}
