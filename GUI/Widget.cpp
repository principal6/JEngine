#include "Widget.h"
#include <cassert>
#include <chrono>
#include "../Core/UTF8.h"
#include "../Core/Shader.h"
#include "../Core/ConstantBuffer.h"

using std::min;
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
	m_Primitive2D.CreateShape(m_Size, SFloat4(1, 1, 1, 1), KDefaultRoundness);
}

void CButton::CreatePreset(const std::string& Name, const SInt2& Size, EButtonPreset ePreset, CWidget* const Parent)
{
	_Create(EWidgetType::Button, Name, Size, Parent);

	m_Primitive2D.CreateShape(m_Size, SFloat4(1, 1, 1, 1), KDefaultRoundness);

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
	m_Primitive2D.CreateShape(m_Size, KDefaultWindowBackgroundColor, Roundness);

	m_TitleBarPrimitive = make_unique<CPrimitive2D>(m_PtrDevice, m_PtrDeviceContext);
	m_TitleBarPrimitive->CreateShape(SInt2(Size.X, KDefaultTitleBarHeight), KDefaultTitleBarActiveColor, Roundness, true);
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
	m_Primitive2D.CreateShape(m_Size, SFloat4(0, 0, 0, 0));
}

void CText::_Draw() const
{
	m_Primitive2D.Draw();
	DrawCaption();
}

void CText::SetBackgroundColor(const SFloat4& Color)
{
	m_Primitive2D.CreateShape(m_Size, Color);
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

	m_Primitive2D.CreateShape(m_Size, KDefaultTextEditBackgroundColor);

	const auto& FontData{ m_BFNTRenderer->GetData() };
	m_CaretPrimitive = make_unique<CPrimitive2D>(m_PtrDevice, m_PtrDeviceContext);
	m_CaretPrimitive->CreateLine(
		{
			SVertex(SFloat4(1, -1, 0, 1), KDefaultCaretColor),
			SVertex(SFloat4(1, -1 -(float)FontData.LineHeight, 0, 1), KDefaultCaretColor),
		}
	);

	SetCaptionAlign(EHorzAlign::Left, EVertAlign::Top);
	SetCaptionColor(KDefaultFontColor);
}

void CTextEdit::_Draw() const
{
	static const steady_clock Clock{};

	m_Primitive2D.Draw();

	{
		UINT OldCount{};
		D3D11_RECT* OldRects{};
		m_PtrDeviceContext->RSGetScissorRects(&OldCount, OldRects);

		auto Parent{ GetParent() };
		const auto& ParentPosition{ (Parent) ? Parent->GetPosition() : SInt2() };
		const auto& ParentOffset{ (Parent) ? Parent->GetOffset() : SInt2() };
		
		D3D11_RECT ScissorRect{};
		ScissorRect.top = ParentPosition.Y + ParentOffset.Y + m_Position.Y + m_Offset.Y;
		ScissorRect.bottom = ScissorRect.top + m_Size.Y;
		ScissorRect.left = ParentPosition.X + ParentOffset.X + m_Position.X + m_Offset.X;
		ScissorRect.right = ScissorRect.left + m_Size.X;
		m_PtrDeviceContext->RSSetScissorRects(1, &ScissorRect);

		DrawCaption(m_Size, SInt2(1, 1));

		m_PtrDeviceContext->RSSetScissorRects(OldCount, OldRects);
	}

	m_PtrVS->Use();
	m_PtrPS->Use();
	m_PtrCBSpace->Use(EShaderType::VertexShader, 0);
	
	if (m_bIsFocused)
	{
		uint32_t Now_ms{ (uint32_t)(Clock.now().time_since_epoch().count() / 1'000'000) };
		if (m_PrevTimePoint_ms == 0) m_PrevTimePoint_ms = Now_ms;
		uint32_t DeltaTime_ms{ Now_ms - m_PrevTimePoint_ms };

		m_CaretTimer += DeltaTime_ms;
		uint32_t CaretBlinkTime{ *m_PtrCaretBlinkTime };
		if (m_CaretTimer <= CaretBlinkTime)
		{
			size_t ByteAt{ ConvertStringAtToByteAt(m_Caption.c_str(), m_CaretAt) };
			m_PtrCBSpaceData->Offset.x += m_BFNTRenderer->CalculateStringWidth(m_Caption.substr(0, ByteAt).c_str());
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

void CTextEdit::InsertChar(const std::string& UTF8_Char)
{
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
}

void CTextEdit::MoveCaret(EDirection eDirection)
{
	if (eDirection == EDirection::Right)
	{
		if (m_CaretAt < GetUTF8StringLength(m_Caption.c_str()))
		{
			++m_CaretAt;
			m_CaretTimer = 0;
		}
	}
	else if (eDirection == EDirection::Left)
	{
		if (m_CaretAt > 0)
		{
			--m_CaretAt;
			m_CaretTimer = 0;
		}
	}
	else if (eDirection == EDirection::Home)
	{
		m_CaretAt = 0;
		m_CaretTimer = 0;
	}
	else if (eDirection == EDirection::End)
	{
		m_CaretAt = (uint32_t)GetUTF8StringLength(m_Caption.c_str());
		m_CaretTimer = 0;
	}
}

void CTextEdit::DeletePreChar()
{
	if (m_CaretAt > 0)
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
	if (m_CaretAt < GetUTF8StringLength(m_Caption.c_str()))
	{
		size_t ByteAt{ ConvertStringAtToByteAt(m_Caption.c_str(), m_CaretAt) };
		size_t PostByteAt{ ConvertStringAtToByteAt(m_Caption.c_str(), m_CaretAt + 1) };
		std::string Pre{ m_Caption.substr(0, ByteAt) };
		std::string Post{ m_Caption.substr(PostByteAt) };
		m_Caption = Pre + Post;
	}
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
