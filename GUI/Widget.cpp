#include "Widget.h"
#include <cassert>
#include <chrono>
#include "../Core/UTF8.h"
#include "../Core/Shader.h"
#include "../Core/ConstantBuffer.h"

using std::string;
using std::wstring;
using std::min;
using std::max;
using std::make_unique;
using std::chrono::steady_clock;

CWidget::CWidget(const SWidgetCtorData& CtorData) :
	m_PtrDevice{ CtorData.Device }, m_PtrDeviceContext{ CtorData.DeviceContext },
	m_PtrVS{ CtorData.VS }, m_PtrPS{ CtorData.PS },
	m_PtrCBSpace{ CtorData.CBSpace }, m_PtrCBSpaceData{ (CWidget::SCBSpaceData*)CtorData.CBSpaceData }, m_PtrWindowSize{ CtorData.WindowSize }
{
	assert(m_PtrDevice);
	assert(m_PtrDeviceContext);

	m_BFNTRenderer = make_unique<CBFNTRenderer>(m_PtrDevice, m_PtrDeviceContext);
	m_BFNTRenderer->Create(CtorData.BFNTRenderer);
}

CWidget::~CWidget()
{
}

void CWidget::_Create(EWidgetType eType, const std::string& Name, const SInt2& Size, CWidget* const Parent)
{
	m_eType = eType;
	m_Name = Name;
	m_Size = m_SelectionSize = Size;
	m_Parent = Parent;
	if (m_Parent && m_Parent->m_umapChildNameToIndex.find(m_Name) == m_Parent->m_umapChildNameToIndex.end())
	{
		m_Position = m_Parent->GetNewChildrenOffset();

		m_Parent->m_vChildren.emplace_back(this);
		m_Parent->m_umapChildNameToIndex[m_Name] = m_Parent->m_vChildren.size() - 1;
	}
}

void CWidget::Draw() const
{
	m_PtrVS->Use();
	m_PtrPS->Use();
	m_PtrCBSpace->Use(EShaderType::VertexShader, 0);

	XMFLOAT2 HalfWindowSize{ m_PtrWindowSize->X * 0.5f, m_PtrWindowSize->Y * 0.5f };
	auto Parent{ GetParent() };
	const auto& ParentPosition{ (Parent) ? Parent->GetPosition() : SInt2() };
	const auto& ParentOffset{ (Parent) ? Parent->GetOffset() : SInt2() };
	m_PtrCBSpaceData->Offset.x = -HalfWindowSize.x + (float)ParentPosition.X + (float)ParentOffset.X + (float)m_Position.X + (float)m_Offset.X;
	m_PtrCBSpaceData->Offset.y = +HalfWindowSize.y - (float)ParentPosition.Y - (float)ParentOffset.Y - (float)m_Position.Y - (float)m_Offset.Y;
	m_PtrCBSpace->Update();

	_Draw();
}

bool CWidget::ShouldDraw() const
{
	if (m_Parent) return m_Parent->ShouldDraw();
	
	if (GetType() == EWidgetType::Window && !((CWindow*)this)->IsOpen()) return false;
	return true;
}

void CWidget::DrawCaption() const
{
	DrawCaption(SInt2(0, 0), SInt2(0, 0));
}

void CWidget::DrawCaption(const SInt2& SizeOverride, const SInt2& Offset) const
{
	if (m_Caption.empty()) return;
	SInt2 _Size{ (SizeOverride.X == 0 && SizeOverride.Y == 0) ? m_Size : SizeOverride };

	const auto& FontData{ m_BFNTRenderer->GetData() };
	auto StringWidth{ m_BFNTRenderer->CalculateStringWidth(m_Caption.c_str()) };

	int32_t XAlign{
		(m_eCaptionHorzAlign == EHorzAlign::Left) ? 0 :
		(m_eCaptionHorzAlign == EHorzAlign::Center) ? (int32_t)((_Size.X / 2) - (StringWidth / 2)) : (int32_t)(_Size.X - StringWidth)
	};
	int32_t YAlign{
		(m_eCaptionVertAlign == EVertAlign::Top) ? (int32_t)FontData.FontSize :
		(m_eCaptionVertAlign == EVertAlign::Middle) ? (int32_t)((_Size.Y / 2) + (FontData.FontSize / 2)) : _Size.Y
	};
	auto Parent{ GetParent() };
	const auto& ParentPosition{ (Parent) ? Parent->GetPosition() : SInt2() };
	const auto& ParentOffset{ (Parent) ? Parent->GetOffset() : SInt2() };
	m_BFNTRenderer->RenderString(m_Caption.c_str(),
		XMFLOAT2
		(
			(float)(ParentPosition.X + ParentOffset.X + m_Position.X + m_Offset.X + Offset.X + XAlign),
			(float)(ParentPosition.Y + ParentOffset.Y + m_Position.Y + m_Offset.Y + Offset.Y + FontData.UnderlinePosition + YAlign)
		),
		m_CaptionColor);
}

void CWidget::Capture(const SInt2& MousePosition) const
{
	m_bCaptured = true;
	m_CapturedMousePosition = MousePosition;
	m_CapturedOffset = m_Offset;
}

void CWidget::Release() const
{
	m_bCaptured = false;
}

bool CWidget::IsCaptured() const
{
	return m_bCaptured;
}

const SInt2& CWidget::GetCapturedMousePosition() const
{
	return m_CapturedMousePosition;
}

const SInt2& CWidget::GetCapturedOffset() const
{
	return m_CapturedOffset;
}

EWidgetType CWidget::GetType() const
{
	return m_eType;
}

const std::string& CWidget::GetName() const
{
	return m_Name;
}

std::string CWidget::GetFullName() const
{
	return ((m_Parent) ? m_Parent->GetName() + GetName() : GetName());
}

EWidgetState CWidget::GetState() const
{
	return m_eState;
}

const SInt2& CWidget::GetPosition() const
{
	return m_Position;
}

const SInt2& CWidget::GetOffset() const
{
	return m_Offset;
}

const SInt2& CWidget::GetSize() const
{
	return m_Size;
}

CWidget* CWidget::GetParent() const
{
	return m_Parent;
}

size_t CWidget::GetChildCount() const
{
	return m_vChildren.size();
}

CWidget* CWidget::GetChild(const std::string& Name) const
{
	if (m_umapChildNameToIndex.find(Name) != m_umapChildNameToIndex.end())
	{
		return m_vChildren[m_umapChildNameToIndex.at(Name)];
	}
	return nullptr;
}

bool CWidget::HasParent() const
{
	return GetParent();
}

bool CWidget::HasChild() const
{
	return GetChildCount();
}

bool CWidget::IsChild(const std::string& Name) const
{
	return (m_umapChildNameToIndex.find(Name) != m_umapChildNameToIndex.end());
}

bool CWidget::IsChild(CWidget* const Widget) const
{
	if (Widget) return IsChild(Widget->GetName());
	return false;
}

bool CWidget::IsFocused() const
{
	return m_bIsFocused;
}

void CWidget::SetOffset(const SInt2& _Offset)
{
	m_Offset = _Offset;
}

void CWidget::SetSelectionSize(const SInt2& SelectionSize)
{
	m_SelectionSize = SelectionSize;
}

void CWidget::SetCaption(const std::string& UTF8_Caption)
{
	m_Caption = UTF8_Caption;
}

void CWidget::SetCaptionAlign(EHorzAlign eHorzAlign)
{
	m_eCaptionHorzAlign = eHorzAlign;
}

void CWidget::SetCaptionAlign(EVertAlign eVertAlign)
{
	m_eCaptionVertAlign = eVertAlign;
}

void CWidget::SetCaptionAlign(EHorzAlign eHorzAlign, EVertAlign eVertAlign)
{
	m_eCaptionHorzAlign = eHorzAlign;
	m_eCaptionVertAlign = eVertAlign;
}

void CWidget::SetCaptionColor(const SFloat4& CaptionColor)
{
	m_CaptionColor.x = CaptionColor.X;
	m_CaptionColor.y = CaptionColor.Y;
	m_CaptionColor.z = CaptionColor.Z;
	m_CaptionColor.w = CaptionColor.W;
}

const std::string& CWidget::GetCaption() const
{
	return m_Caption;
}

void CWidget::UpdateState(const SInt2& MousePosition, EEventType& eEventType, bool bMouseDown,
	CWidget* const LastMouseDownWidget, CWidget* const FocusedWidget)
{
	bool bIsActive{ (IsChild(FocusedWidget) ? true : this == FocusedWidget) };
	m_bIsFocused = (this == FocusedWidget);

	if (IsMouseInside(MousePosition))
	{
		// Event: Clicked
		if (eEventType == EEventType::MouseUp)
		{
			if (GetState() == EWidgetState::Pressed)
			{
				eEventType = EEventType::Clicked; // @important
			}
		}

		if (bMouseDown && this == LastMouseDownWidget) // continue to be pressed
		{
			SetState(EWidgetState::Pressed);
		}
		else if (eEventType == EEventType::MouseDown && IsMouseInside(MousePosition)) // first pressed
		{
			SetState(EWidgetState::Pressed);
		}
		else // not pressed
		{
			SetState(EWidgetState::Hover);
		}
	}
	else
	{
		SetState(EWidgetState::Normal);
	}

	if (bIsActive && !m_bPrevIsActive)
	{
		_SetActive();
	}
	else if (!bIsActive && m_bPrevIsActive)
	{
		_SetInactive();
	}
	m_bPrevIsActive = bIsActive;
}

void CWidget::SetState(EWidgetState eState)
{
	m_eState = eState;

	_SetState();
}

void CWidget::SetStateTexCoordRange(EWidgetState eWidgetState, const SFloat4& TexCoordRange)
{
	switch (eWidgetState)
	{
	case EWidgetState::Normal:
		m_TexCoordRangeNormal = TexCoordRange;
		break;
	case EWidgetState::Hover:
		m_TexCoordRangeHover = TexCoordRange;
		break;
	case EWidgetState::Pressed:
		m_TexCoordRangePressed = TexCoordRange;
		break;
	default:
		break;
	}
}

const SInt2& CWidget::GetNewChildrenOffset() const
{
	return m_NewChildrenOffset;
}

bool CWidget::IsMouseInside(const SInt2& MousePosition)
{
	int32_t HalfSizeX{ m_Size.X / 2 };
	int32_t HalfSizeY{ m_Size.Y / 2 };
	int32_t HalfSelectionSizeX{ m_SelectionSize.X / 2 };
	int32_t HalfSelectionSizeY{ m_SelectionSize.Y / 2 };
	auto Parent{ GetParent() };
	const auto& ParentPosition{ (Parent) ? Parent->GetPosition() : SInt2() };
	const auto& ParentOffset{ (Parent) ? Parent->GetOffset() : SInt2() };
	if (MousePosition.X >= ParentPosition.X + ParentOffset.X + m_Position.X + m_Offset.X + HalfSizeX - HalfSelectionSizeX &&
		MousePosition.X <= ParentPosition.X + ParentOffset.X + m_Position.X + m_Offset.X + HalfSizeX + HalfSelectionSizeX &&
		MousePosition.Y >= ParentPosition.Y + ParentOffset.Y + m_Position.Y + m_Offset.Y + HalfSizeY - HalfSelectionSizeY &&
		MousePosition.Y <= ParentPosition.Y + ParentOffset.Y + m_Position.Y + m_Offset.Y + HalfSizeY + HalfSelectionSizeY)
	{
		return true;
	}
	return false;
}

// CButton
CButton::CButton(const SWidgetCtorData& CtorData) : CWidget(CtorData)
{
}

CButton::~CButton()
{
}

void CButton::Create(const std::string& Name, const SInt2& Size, CWidget* const Parent)
{
	_Create(EWidgetType::Button, Name, Size, Parent);
	m_Primitive2D.CreateRectangle(m_Size, SFloat4(1, 1, 1, 1), KDefaultRoundness);
}

void CButton::CreatePreset(const std::string& Name, const SInt2& Size, EButtonPreset ePreset, CWidget* const Parent)
{
	_Create(EWidgetType::Button, Name, Size, Parent);

	m_Primitive2D.CreateRectangle(m_Size, SFloat4(1, 1, 1, 1), KDefaultRoundness);

	m_PresetPrimitive = make_unique<CPrimitive2D>(m_PtrDevice, m_PtrDeviceContext);
	switch (ePreset)
	{
	case EButtonPreset::Close:
	{
		m_Position.Y = 0; // @important

		float SpaceX{ Size.X * 0.25f };
		float SpaceY{ Size.Y * 0.25f };
		float _SizeX{ Size.X * (1.0f - 0.5f) };
		float _SizeY{ Size.Y * (1.0f - 0.5f) };
		m_PresetPrimitive->CreateLine(
			{
				SVertex(SFloat4(SpaceX			, -SpaceY			, 0, 1), KDefaultIconColor),
				SVertex(SFloat4(SpaceX + _SizeX	, -SpaceY - _SizeY	, 0, 1), KDefaultIconColor),
				SVertex(SFloat4(SpaceX + _SizeX	, -SpaceY			, 0, 1), KDefaultIconColor),
				SVertex(SFloat4(SpaceX			, -SpaceY - _SizeY	, 0, 1), KDefaultIconColor)
			}
		);
		break;
	}
	default:
		break;
	}
}

void CButton::_Draw() const
{
	m_Primitive2D.Draw();
	if (m_PresetPrimitive)
	{
		m_PresetPrimitive->Draw();
		return;
	}
	DrawCaption();
}

void CButton::_SetActive()
{
}

void CButton::_SetInactive()
{
}

void CButton::_SetState()
{
	SFloat4 StateColor{ (m_eState == EWidgetState::Normal) ? KDefaultButtonNormalColor :
		(m_eState == EWidgetState::Hover) ? KDefaultButtonHoverColor : KDefaultButtonPressedColor };
	auto& Vertices{ m_Primitive2D.GetVertices() };
	for (auto& Vertex : Vertices)
	{
		Vertex.ColorTex = StateColor;
	}
	m_Primitive2D.UpdateBuffers();
}

// CImage
CImage::CImage(const SWidgetCtorData& CtorData) : CWidget(CtorData)
{
}

CImage::~CImage()
{
}

void CImage::Create(const std::string& Name, const SInt2& Size, const SFloat4& TexCoordRange, CWidget* const Parent)
{
	_Create(EWidgetType::Image, Name, Size, Parent);
	m_Primitive2D.CreateImage(m_Size, TexCoordRange);
}

void CImage::SetSource(ID3D11ShaderResourceView* const SRV)
{
	auto& Vertices{ m_Primitive2D.GetVertices() };
	Vertices[0].ColorTex.W = (SRV) ? -2.0f : -1.0f;
	Vertices[1].ColorTex.W = (SRV) ? -2.0f : -1.0f;
	Vertices[2].ColorTex.W = (SRV) ? -2.0f : -1.0f;
	Vertices[3].ColorTex.W = (SRV) ? -2.0f : -1.0f;

	m_Primitive2D.UpdateBuffers();
	m_SRV = SRV;
}

void CImage::SetSource(ID3D11ShaderResourceView* const SRV, const SFloat4& TexCoordRange)
{
	auto& Vertices{ m_Primitive2D.GetVertices() };
	Vertices[0].ColorTex = SFloat4(TexCoordRange.X, TexCoordRange.Y, 0, -2.0f);
	Vertices[1].ColorTex = SFloat4(TexCoordRange.Z, TexCoordRange.Y, 0, -2.0f);
	Vertices[2].ColorTex = SFloat4(TexCoordRange.X, TexCoordRange.W, 0, -2.0f);
	Vertices[3].ColorTex = SFloat4(TexCoordRange.Z, TexCoordRange.W, 0, -2.0f);

	SetSource(SRV);
}

ID3D11ShaderResourceView* CImage::GetSource() const
{
	return m_SRV;
}

void CImage::_Draw() const
{
	m_Primitive2D.Draw();
}

void CImage::_SetActive()
{
}

void CImage::_SetInactive()
{
}

void CImage::_SetState()
{
}

// CImageButton
CImageButton::CImageButton(const SWidgetCtorData& CtorData) : CWidget(CtorData)
{
}

CImageButton::~CImageButton()
{
}

void CImageButton::Create(const std::string& Name, const SInt2& Size, CWidget* const Parent)
{
	_Create(EWidgetType::ImageButton, Name, Size, Parent);
	m_Primitive2D.CreateImage(m_Size, SFloat4(0, 0, 0, 0));
}

void CImageButton::_Draw() const
{
	m_Primitive2D.Draw();
	DrawCaption();
}

void CImageButton::_SetActive()
{
}

void CImageButton::_SetInactive()
{
}

void CImageButton::_SetState()
{
	SFloat4 StateTexCoordRange{ (m_eState == EWidgetState::Normal) ? m_TexCoordRangeNormal :
		(m_eState == EWidgetState::Hover) ? m_TexCoordRangeHover : m_TexCoordRangePressed };

	auto& Vertices{ m_Primitive2D.GetVertices() };
	Vertices[0].ColorTex = SFloat4(StateTexCoordRange.X, StateTexCoordRange.Y, 0, -1);
	Vertices[1].ColorTex = SFloat4(StateTexCoordRange.Z, StateTexCoordRange.Y, 0, -1);
	Vertices[2].ColorTex = SFloat4(StateTexCoordRange.X, StateTexCoordRange.W, 0, -1);
	Vertices[3].ColorTex = SFloat4(StateTexCoordRange.Z, StateTexCoordRange.W, 0, -1);

	m_Primitive2D.UpdateBuffers();
}

CWindow::CWindow(const SWidgetCtorData& CtorData) : CWidget(CtorData)
{
}

CWindow::~CWindow()
{
}

void CWindow::CreateImage(const std::string& Name, const SInt2& Size, const SFloat4& TexCoordRange, CWidget* const Parent)
{
	_Create(EWidgetType::Window, Name, Size, Parent);
	m_Primitive2D.CreateImage(m_Size, TexCoordRange);
}

void CWindow::CreatePrimitive(const std::string& Name, const SInt2& Size, float Roundness, CWidget* const Parent)
{
	_Create(EWidgetType::Window, Name, Size, Parent);
	m_Primitive2D.CreateRectangle(m_Size, KDefaultWindowBackgroundColor, Roundness);

	m_TitleBarPrimitive = make_unique<CPrimitive2D>(m_PtrDevice, m_PtrDeviceContext);
	m_TitleBarPrimitive->CreateRectangle(SInt2(Size.X, KDefaultTitleBarHeight), KDefaultTitleBarActiveColor, Roundness, true);
	SetTitleBar(SInt2(0, 0), SInt2(Size.X - KDefaultTitleBarHeight, KDefaultTitleBarHeight));

	m_NewChildrenOffset.Y = KDefaultTitleBarHeight;

	SetCaptionAlign(EHorzAlign::Left, EVertAlign::Middle);
	SetCaptionColor(KDefaultFontColor);
}

void CWindow::Open()
{
	m_bIsOpen = true;
}

void CWindow::Close()
{
	m_bIsOpen = false;
}

bool CWindow::IsOpen()
{
	return m_bIsOpen;
}

void CWindow::SetTitleBar(const SInt2& TitleBarLeftTop, const SInt2& TitleBarSize)
{
	m_TitleBarLeftTop = TitleBarLeftTop;
	m_TitleBarSize = TitleBarSize;
}

bool CWindow::IsMouseOnTitleBar(const SInt2& MousePosition) const
{
	auto Parent{ GetParent() };
	const auto& ParentPosition{ (Parent) ? Parent->GetPosition() : SInt2() };
	const auto& ParentOffset{ (Parent) ? Parent->GetOffset() : SInt2() };
	if (MousePosition.X >= ParentPosition.X + ParentOffset.X + m_Position.X + m_Offset.X + m_TitleBarLeftTop.X						&&
		MousePosition.X <= ParentPosition.X + ParentOffset.X + m_Position.X + m_Offset.X + m_TitleBarLeftTop.X + m_TitleBarSize.X	&&
		MousePosition.Y >= ParentPosition.Y + ParentOffset.Y + m_Position.Y + m_Offset.Y + m_TitleBarLeftTop.Y						&&
		MousePosition.Y <= ParentPosition.Y + ParentOffset.Y + m_Position.Y + m_Offset.Y + m_TitleBarLeftTop.Y + m_TitleBarSize.Y)
	{
		return true;
	}
	return false;
}

void CWindow::_Draw() const
{
	m_Primitive2D.Draw();

	if (m_TitleBarPrimitive) m_TitleBarPrimitive->Draw();

	DrawCaption(SInt2(m_Size.X - KDefaultTitleBarHeight, KDefaultTitleBarHeight), SInt2(KDefaultTitleBarCaptionIndent, 0));
}

void CWindow::_SetActive()
{
	auto& Vertices{ m_TitleBarPrimitive->GetVertices() };
	for (auto& Vertex : Vertices)
	{
		Vertex.ColorTex = KDefaultTitleBarActiveColor;
	}
	m_TitleBarPrimitive->UpdateBuffers();
}

void CWindow::_SetInactive()
{
	auto& Vertices{ m_TitleBarPrimitive->GetVertices() };
	for (auto& Vertex : Vertices)
	{
		Vertex.ColorTex = KDefaultTitleBarInactiveColor;
	}
	m_TitleBarPrimitive->UpdateBuffers();
}

void CWindow::_SetState()
{
}

CText::CText(const SWidgetCtorData& CtorData) : CWidget(CtorData)
{
}

CText::~CText()
{
}

void CText::Create(const std::string& Name, const SInt2& Size, CWidget* const Parent)
{
	_Create(EWidgetType::Text, Name, Size, Parent);
	m_Primitive2D.CreateRectangle(m_Size, SFloat4(0, 0, 0, 0));
}

void CText::_Draw() const
{
	m_Primitive2D.Draw();
	DrawCaption();
}

void CText::SetBackgroundColor(const SFloat4& Color)
{
	m_Primitive2D.CreateRectangle(m_Size, Color);
}

void CText::_SetActive()
{
}

void CText::_SetInactive()
{
}

void CText::_SetState()
{
}

CTextEdit::CTextEdit(const SWidgetCtorData& CtorData) : CWidget(CtorData)
{
}

CTextEdit::~CTextEdit()
{
}

void CTextEdit::Create(const std::string& Name, const SInt2& Size, CWidget* const Parent)
{
	_Create(EWidgetType::TextEdit, Name, Size, Parent);

	m_Primitive2D.CreateRectangle(m_Size, KDefaultTextEditBackgroundColor);

	const auto& FontData{ m_BFNTRenderer->GetData() };
	m_CaretPrimitive = make_unique<CPrimitive2D>(m_PtrDevice, m_PtrDeviceContext);
	m_CaretPrimitive->CreateLine(
		{
			SVertex(SFloat4(KSpaceHorz + 1, -KSpaceVert, 0, 1), KDefaultCaretColor),
			SVertex(SFloat4(KSpaceHorz + 1, -KSpaceVert -(float)FontData.LineHeight, 0, 1), KDefaultCaretColor),
		}
	);

	m_SelectionPrimitive = make_unique<CPrimitive2D>(m_PtrDevice, m_PtrDeviceContext);
	m_SelectionPrimitive->CreateRectangle(SInt2(10, FontData.LineHeight), KDefaultSelectionColor);

	SetCaptionAlign(EHorzAlign::Left, EVertAlign::Top);
	SetCaptionColor(KDefaultFontColor);
	MoveCaret(EDirection::Left);
}

void CTextEdit::_Draw() const
{
	static const steady_clock Clock{};

	// Draw background
	m_Primitive2D.Draw();

	// Set scissor rect
	UINT OldScissorRectCount{};
	D3D11_RECT* OldScissorRects{};
	{
		m_PtrDeviceContext->RSGetScissorRects(&OldScissorRectCount, OldScissorRects);

		auto Parent{ GetParent() };
		const auto& ParentPosition{ (Parent) ? Parent->GetPosition() : SInt2() };
		const auto& ParentOffset{ (Parent) ? Parent->GetOffset() : SInt2() };

		D3D11_RECT ScissorRect{};
		ScissorRect.top = ParentPosition.Y + ParentOffset.Y + m_Position.Y + m_Offset.Y;
		ScissorRect.bottom = ScissorRect.top + m_Size.Y;
		ScissorRect.left = ParentPosition.X + ParentOffset.X + m_Position.X + m_Offset.X + KSpaceHorz;
		ScissorRect.right = ScissorRect.left + m_Size.X - KSpaceHorz * 2;
		m_PtrDeviceContext->RSSetScissorRects(1, &ScissorRect);
	}

	// Draw text
	DrawCaption(m_Size, SInt2(KSpaceHorz + m_StringOffsetX, KSpaceVert));

	const int32_t StringWidth{ (int32_t)m_BFNTRenderer->CalculateStringWidth(m_Caption.c_str()) };
	const int32_t AlignOffsetX{ (m_eCaptionHorzAlign == EHorzAlign::Center) ? (m_Size.X / 2) - (StringWidth / 2) : 0 };

	float OldSpaceOffsetX{ m_PtrCBSpaceData->Offset.x };
	float OldSpaceOffsetY{ m_PtrCBSpaceData->Offset.y };

	m_PtrVS->Use();
	m_PtrPS->Use();
	m_PtrCBSpace->Use(EShaderType::VertexShader, 0);

	// Draw selection
	if (HasSelection())
	{
		if (m_CaretAt == m_SelectionStart)
		{
			m_PtrCBSpaceData->Offset.x = OldSpaceOffsetX + KSpaceHorz + m_CaretOffsetX + m_StringOffsetX + AlignOffsetX;
		}
		else // m_CaretAt == m_SelectionEnd
		{
			m_PtrCBSpaceData->Offset.x = OldSpaceOffsetX + KSpaceHorz + m_CaretOffsetX + m_StringOffsetX + AlignOffsetX - m_SelectionWidth;
		}
		m_PtrCBSpaceData->Offset.y = OldSpaceOffsetY - KSpaceVert;

		m_PtrCBSpace->Update();

		m_SelectionPrimitive->Draw();
	}

	// Restore scissor rect
	m_PtrDeviceContext->RSSetScissorRects(OldScissorRectCount, OldScissorRects);

	// Draw caret
	if (m_bIsFocused)
	{
		const uint32_t Now_ms{ (uint32_t)(Clock.now().time_since_epoch().count() / 1'000'000) };
		const uint32_t DeltaTime_ms{ Now_ms - m_PrevTimePoint_ms };
		const uint32_t CaretBlinkTime{ *m_PtrCaretBlinkTime };
		m_CaretTimer += DeltaTime_ms;
		if (m_CaretTimer <= CaretBlinkTime)
		{
			m_PtrCBSpaceData->Offset.x = OldSpaceOffsetX + m_CaretOffsetX + m_StringOffsetX + AlignOffsetX;
			m_PtrCBSpaceData->Offset.y = OldSpaceOffsetY;
			m_PtrCBSpace->Update();

			m_CaretPrimitive->Draw();
		}
		else if (m_CaretTimer > CaretBlinkTime * 2)
		{
			m_CaretTimer = 0;
		}

		m_PrevTimePoint_ms = Now_ms;
	}
}

bool CTextEdit::InsertChar(const std::string& UTF8_Char)
{
	if (GetUTF8StringLength(m_Caption.c_str()) >= KMaxStringLength) return false;

	string _UTF8_Char{ UTF8_Char };
	size_t ByteCount{ GetUTF8CharacterByteCount(_UTF8_Char[0]) };
	if (_UTF8_Char.size() > ByteCount) _UTF8_Char = _UTF8_Char.substr(0, ByteCount);

	if (m_Caption.size())
	{
		size_t ByteAt{ ConvertStringAtToByteAt(m_Caption.c_str(), m_CaretAt) };
		std::string Pre{ m_Caption.substr(0, ByteAt) };
		std::string Post{ m_Caption.substr(ByteAt) };
		m_Caption = Pre + UTF8_Char + Post;
	}
	else
	{
		m_Caption = UTF8_Char;
	}
	MoveCaret(EDirection::Right);
	return true;
}

void CTextEdit::InsertString(const std::string& UTF8_String)
{
	size_t ByteAt{};
	while (ByteAt < UTF8_String.size())
	{
		size_t ByteCount{ GetUTF8CharacterByteCount(UTF8_String[ByteAt]) };
		if (!InsertChar(UTF8_String.substr(ByteAt, ByteCount))) return;
		ByteAt += ByteCount;
	}
}

void CTextEdit::MoveCaret(EDirection eDirection, bool bShouldDeselect)
{
	if (bShouldDeselect)
	{
		Deselect();
	}

	const int32_t _SizeX{ m_Size.X - KSpaceHorz * 2 };
	switch (eDirection)
	{
	case CTextEdit::EDirection::Left:
		if (m_CaretAt > 0)
		{
			--m_CaretAt;
			m_CaretTimer = 0;

			size_t ByteAt{ ConvertStringAtToByteAt(m_Caption.c_str(), m_CaretAt) };
			m_CaretOffsetX = (int32_t)m_BFNTRenderer->CalculateStringWidth(m_Caption.substr(0, ByteAt).c_str());
			int32_t DeltaStringOffsetX{ (m_CaretOffsetX < abs(m_StringOffsetX)) ? abs(m_StringOffsetX) - m_CaretOffsetX : 0 };
			m_StringOffsetX += DeltaStringOffsetX;
		}
		break;
	case CTextEdit::EDirection::Right:
		if (m_CaretAt < (uint32_t)GetUTF8StringLength(m_Caption.c_str()))
		{
			++m_CaretAt;
			m_CaretTimer = 0;

			size_t ByteAt{ ConvertStringAtToByteAt(m_Caption.c_str(), m_CaretAt) };
			m_CaretOffsetX = (int32_t)m_BFNTRenderer->CalculateStringWidth(m_Caption.substr(0, ByteAt).c_str());
			int32_t DeltaStringOffsetX{ (_SizeX - m_CaretOffsetX - m_StringOffsetX < 0) ? _SizeX - m_CaretOffsetX - m_StringOffsetX : 0 };
			m_StringOffsetX += DeltaStringOffsetX;
		}
		break;
	case CTextEdit::EDirection::Home:
		m_CaretAt = 0;
		m_CaretTimer = 0;

		m_CaretOffsetX = 0;
		m_StringOffsetX = 0;
		break;
	case CTextEdit::EDirection::End:
		m_CaretAt = (uint32_t)GetUTF8StringLength(m_Caption.c_str());
		m_CaretTimer = 0;

		m_CaretOffsetX = (int32_t)m_BFNTRenderer->CalculateStringWidth(m_Caption.c_str());
		m_StringOffsetX = ((_SizeX - m_CaretOffsetX < 0) ? _SizeX - m_CaretOffsetX : 0);
		break;
	default:
		break;
	}

	if (m_eCaptionHorzAlign == EHorzAlign::Center)
	{
		m_StringOffsetX = -KSpaceHorz;
	}
}

void CTextEdit::MoveCaret(const SInt2& MousePosition)
{
	uint32_t NewCaretAt{ _GetCaretAtFromMousePosition(MousePosition) };
	if (NewCaretAt < m_CaretAt)
	{
		while (NewCaretAt < m_CaretAt)
		{
			MoveCaret(EDirection::Left);
		}
	}
	else if (NewCaretAt > m_CaretAt)
	{
		while (NewCaretAt > m_CaretAt)
		{
			MoveCaret(EDirection::Right);
		}
	}
}

void CTextEdit::Select(EDirection eDirection)
{
	switch (eDirection)
	{
	case CTextEdit::EDirection::Left:
	{
		uint32_t OldCaretAt{ m_CaretAt };
		MoveCaret(EDirection::Left, false);
		if (m_CaretAt != OldCaretAt)
		{
			if (m_SelectionStart == m_SelectionEnd)
			{
				m_SelectionStart = m_CaretAt;
				m_SelectionEnd = OldCaretAt;
			}
			else
			{
				if (m_CaretAt < m_SelectionStart)
				{
					--m_SelectionStart;
				}
				else
				{
					--m_SelectionEnd;
				}
			}
		}
		break;
	}
	case CTextEdit::EDirection::Right:
	{
		uint32_t OldCaretAt{ m_CaretAt };
		MoveCaret(EDirection::Right, false);
		if (m_CaretAt != OldCaretAt)
		{
			if (m_SelectionStart == m_SelectionEnd)
			{
				m_SelectionStart = OldCaretAt;
				m_SelectionEnd = m_CaretAt;
			}
			else
			{
				if (m_CaretAt > m_SelectionEnd)
				{
					++m_SelectionEnd;
				}
				else
				{
					++m_SelectionStart;
				}
			}
		}
		break;
	}
	case CTextEdit::EDirection::Home:
	{
		uint32_t OldCaretAt{ m_CaretAt };
		MoveCaret(EDirection::Home, false);
		if (m_CaretAt != OldCaretAt)
		{
			if (m_SelectionStart == m_SelectionEnd)
			{
				m_SelectionEnd = OldCaretAt;
			}
			else
			{
				if (OldCaretAt > m_SelectionStart)
				{
					m_SelectionEnd = m_SelectionStart;
				}
			}
			m_SelectionStart = 0;
		}
		break;
	}
	case CTextEdit::EDirection::End:
	{
		uint32_t OldCaretAt{ m_CaretAt };
		MoveCaret(EDirection::End, false);
		if (m_CaretAt != OldCaretAt)
		{
			if (m_SelectionStart == m_SelectionEnd)
			{
				m_SelectionStart = OldCaretAt;
			}
			else
			{
				if (OldCaretAt < m_SelectionEnd)
				{
					m_SelectionStart = m_SelectionEnd;
				}
			}
			m_SelectionEnd = m_CaretAt;
		}
		break;
	}
	default:
		return; // @
	}

	if (HasSelection())
	{
		size_t SelectionStartByte{ ConvertStringAtToByteAt(m_Caption.c_str(), m_SelectionStart) };
		size_t SelectionEndByte{ ConvertStringAtToByteAt(m_Caption.c_str(), m_SelectionEnd) };
		int32_t SelectionStartWidth{ (int32_t)m_BFNTRenderer->CalculateStringWidth(m_Caption.substr(0, SelectionStartByte).c_str()) };
		int32_t SelectionEndWidth{ (int32_t)m_BFNTRenderer->CalculateStringWidth(m_Caption.substr(0, SelectionEndByte).c_str()) };
		m_SelectionWidth = SelectionEndWidth - SelectionStartWidth;

		auto& Vertices{ m_SelectionPrimitive->GetVertices() };
		Vertices[1].Position.X = (float)m_SelectionWidth;
		Vertices[3].Position.X = (float)m_SelectionWidth;
		m_SelectionPrimitive->UpdateBuffers();
	}
}

void CTextEdit::Select(const SInt2& MousePosition)
{
	uint32_t NewCaretAt{ _GetCaretAtFromMousePosition(MousePosition) };
	if (NewCaretAt < m_CaretAt)
	{
		while (NewCaretAt < m_CaretAt)
		{
			Select(EDirection::Left);
		}
	}
	else if (NewCaretAt > m_CaretAt)
	{
		while (NewCaretAt > m_CaretAt)
		{
			Select(EDirection::Right);
		}
	}
}

void CTextEdit::Deselect()
{
	m_SelectionStart = m_SelectionEnd = 0;
}

void CTextEdit::DeletePreChar()
{
	if (HasSelection())
	{
		DeleteSelection();
	}
	else if (m_CaretAt > 0)
	{
		size_t ByteAt{ ConvertStringAtToByteAt(m_Caption.c_str(), m_CaretAt) };
		size_t PreByteAt{ ConvertStringAtToByteAt(m_Caption.c_str(), m_CaretAt - 1) };
		std::string Pre{ m_Caption.substr(0, PreByteAt) };
		std::string Post{ m_Caption.substr(ByteAt) };
		m_Caption = Pre + Post;

		MoveCaret(EDirection::Left);
	}
}

void CTextEdit::DeletePostChar()
{
	if (HasSelection())
	{
		DeleteSelection();
	}
	else if (m_CaretAt < GetUTF8StringLength(m_Caption.c_str()))
	{
		size_t ByteAt{ ConvertStringAtToByteAt(m_Caption.c_str(), m_CaretAt) };
		size_t PostByteAt{ ConvertStringAtToByteAt(m_Caption.c_str(), m_CaretAt + 1) };
		std::string Pre{ m_Caption.substr(0, ByteAt) };
		std::string Post{ m_Caption.substr(PostByteAt) };
		m_Caption = Pre + Post;

		if (m_CaretAt < GetUTF8StringLength(m_Caption.c_str()))
		{
			MoveCaret(EDirection::Right);
			MoveCaret(EDirection::Left);
		}
		else
		{
			MoveCaret(EDirection::Left);
			MoveCaret(EDirection::Right);
		}
	}
}

void CTextEdit::DeleteSelection()
{
	if (HasSelection())
	{
		uint32_t OldCaretAt{ m_CaretAt };
		uint32_t OldSelectionStart{ m_SelectionStart };

		size_t StartByteAt{ ConvertStringAtToByteAt(m_Caption.c_str(), m_SelectionStart) };
		size_t EndByteAt{ ConvertStringAtToByteAt(m_Caption.c_str(), m_SelectionEnd) };
		std::string Pre{ m_Caption.substr(0, StartByteAt) };
		std::string Post{ m_Caption.substr(EndByteAt) };
		m_Caption = Pre + Post;

		Deselect();

		if (OldCaretAt > OldSelectionStart)
		{
			uint32_t Difference{ OldCaretAt - OldSelectionStart };
			for (uint32_t i = 0; i < Difference; ++i)
			{
				MoveCaret(EDirection::Left);
			}
		}
	}
}

bool CTextEdit::HasSelection() const
{
	return (m_SelectionStart != m_SelectionEnd);
}

uint32_t CTextEdit::GetSelectionLength() const
{
	return m_SelectionEnd - m_SelectionStart;
}

uint32_t CTextEdit::GetSelectionByteLength() const
{
	size_t SelectionStartByteAt{ ConvertStringAtToByteAt(m_Caption.c_str(), m_SelectionStart) };
	size_t SelectionEndByteAt{ ConvertStringAtToByteAt(m_Caption.c_str(), m_SelectionEnd) };
	return (uint32_t)(SelectionEndByteAt - SelectionStartByteAt);
}

std::string CTextEdit::GetSelection() const
{
	size_t SelectionStartByteAt{ ConvertStringAtToByteAt(m_Caption.c_str(), m_SelectionStart) };
	size_t SelectionEndByteAt{ ConvertStringAtToByteAt(m_Caption.c_str(), m_SelectionEnd) };
	return m_Caption.substr(SelectionStartByteAt, SelectionEndByteAt - SelectionStartByteAt);
}

bool CTextEdit::CopyToClipboard()
{
	bool bResult{ false };
	if (HasSelection() && OpenClipboard(nullptr))
	{
		size_t WideStringLength{ GetSelectionLength() };
		wchar_t* _WideString{};
		ConvertUTF8ToWide(GetSelection().c_str(), &_WideString);
		wstring WideString{ _WideString };
		if (_WideString)
		{
			delete[] _WideString;
			_WideString = nullptr;
		}
		
		HGLOBAL Memory{ GlobalAlloc(GMEM_MOVEABLE, (WideString.size() + 1) * sizeof(wchar_t)) };
		if (Memory)
		{
			wchar_t* Locked{ (wchar_t*)GlobalLock(Memory) };
			if (Locked)
			{
				memcpy(Locked, WideString.c_str(), WideString.size() * sizeof(wchar_t));
				Locked[WideString.size()] = 0;    // null character 

				GlobalUnlock(Locked);

				EmptyClipboard();

				SetClipboardData(CF_UNICODETEXT, Memory);

				bResult = true;
			}

			GlobalFree(Memory);
		}

		CloseClipboard();
	}

	return bResult;
}

bool CTextEdit::CutToClipboard()
{
	if (CopyToClipboard())
	{
		DeleteSelection();
		return true;
	}
	return false;
}

bool CTextEdit::PasteFromClipboard()
{
	bool bResult{ false };
	if (OpenClipboard(nullptr))
	{
		HANDLE ClipboardData{ GetClipboardData(CF_UNICODETEXT) };
		wchar_t* Locked{ (wchar_t*)GlobalLock(ClipboardData) };
		if (Locked)
		{
			wstring WideString{};
			size_t WideStringLength{ wcslen(Locked) };
			for (size_t i = 0; i < WideStringLength; ++i)
			{
				WideString += Locked[i];
			}
			GlobalUnlock(Locked);

			DeleteSelection();

			char* UTF8{};
			ConvertWideToUTF8(WideString.c_str(), &UTF8);
			string UTF8String{ UTF8 };
			if (UTF8)
			{
				delete[] UTF8;
				UTF8 = nullptr;
			}

			InsertString(UTF8String);

			bResult = true;
		}

		CloseClipboard();
	}

	return bResult;
}

uint32_t CTextEdit::_GetCaretAtFromMousePosition(const SInt2& MousePosition) const
{
	SInt2 ClientPosition{ MousePosition.X - (m_Position.X + m_Offset.X), MousePosition.Y + (m_Position.Y + m_Offset.Y) };
	if (HasParent())
	{
		const auto& ParentPosition{ GetParent()->GetPosition() };
		const auto& ParentOffset{ GetParent()->GetOffset() };
		ClientPosition.X -= (ParentPosition.X + ParentOffset.X);
		ClientPosition.Y += (ParentPosition.Y + ParentOffset.Y);
	}
	int32_t OffsetX{ ClientPosition.X - m_StringOffsetX };
	uint32_t NewCaretAt{ m_BFNTRenderer->CalculateCharacterAtFromOffsetX(m_Caption.c_str(), OffsetX) };
	return NewCaretAt;
}

void CTextEdit::_SetActive()
{
	m_CaretTimer = 0;
}

void CTextEdit::_SetInactive()
{
}

void CTextEdit::_SetState()
{
}
