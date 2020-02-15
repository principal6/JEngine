#include "Widget.h"
#include <cassert>

using std::make_unique;

CWidget::CWidget(ID3D11Device* const Device, ID3D11DeviceContext* const DeviceContext, CBFNTRenderer* const BFNTRenderer) :
	m_PtrDevice{ Device }, m_PtrDeviceContext{ DeviceContext }
{
	assert(m_PtrDevice);
	assert(m_PtrDeviceContext);

	m_BFNTRenderer = make_unique<CBFNTRenderer>(m_PtrDevice, m_PtrDeviceContext);
	m_BFNTRenderer->Create(BFNTRenderer);
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
		m_Parent->m_vChildren.emplace_back(this);
		m_Parent->m_umapChildNameToIndex[m_Name] = m_Parent->m_vChildren.size() - 1;
	}
}

bool CWidget::ShouldDraw() const
{
	if (m_Parent) return m_Parent->ShouldDraw();
	
	if (GetType() == EWidgetType::Window && !((CWindow*)this)->IsOpen()) return false;
	return true;
}

void CWidget::DrawCaption() const
{
	if (m_Caption.empty()) return;

	const auto& FontData{ m_BFNTRenderer->GetData() };
	auto StringWidth{ m_BFNTRenderer->CalculateStringWidth(m_Caption.c_str()) };

	int32_t XAlign{
		(m_eCaptionHorzAlign == EHorzAlign::Left) ? 0 :
		(m_eCaptionHorzAlign == EHorzAlign::Center) ? (int32_t)((m_Size.X / 2) - (StringWidth / 2)) : (int32_t)(m_Size.X - StringWidth)
	};
	int32_t YAlign{
		(m_eCaptionVertAlign == EVertAlign::Top) ? (int32_t)FontData.FontSize :
		(m_eCaptionVertAlign == EVertAlign::Middle) ? (int32_t)((m_Size.Y / 2) + (FontData.FontSize / 2)) : m_Size.Y
	};
	auto Parent{ GetParent() };
	const auto& ParentPosition{ (Parent) ? Parent->GetPosition() : SInt2() };
	const auto& ParentOffset{ (Parent) ? Parent->GetOffset() : SInt2() };
	m_BFNTRenderer->RenderString(m_Caption.c_str(),
		XMFLOAT2
		(
			(float)(ParentPosition.X + ParentOffset.X + m_Position.X + m_Offset.X + XAlign),
			(float)(ParentPosition.Y + ParentOffset.Y + m_Position.Y + m_Offset.Y + FontData.UnderlinePosition + YAlign)
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

void CWidget::UpdateState(const SInt2& MousePosition, EEventType& eEventType, bool bMouseDown, CWidget* LastMouseDownWidget)
{
	if (IsMouseInside(MousePosition))
	{
		// Event: Clicked
		if (eEventType == EEventType::MouseUp)
		{
			if (GetState() == EWidgetState::Pressed) eEventType = EEventType::Clicked; // @important
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
CButton::CButton(ID3D11Device* const Device, ID3D11DeviceContext* const DeviceContext, CBFNTRenderer* const BFNTRenderer) :
	CWidget(Device, DeviceContext, BFNTRenderer)
{
}

CButton::~CButton()
{
}

void CButton::Create(const std::string& Name, const SInt2& Size, CWidget* const Parent)
{
	_Create(EWidgetType::Button, Name, Size, Parent);
	m_Internal2D.CreateColor(m_Size, SFloat4(1, 1, 1, 1), KDefaultRoundness);
}

void CButton::Draw() const
{
	m_Internal2D.Draw();
	DrawCaption();
}

void CButton::_SetState()
{
	SFloat4 StateColor{ (m_eState == EWidgetState::Normal) ? KDefaultButtonNormal :
		(m_eState == EWidgetState::Hover) ? KDefaultButtonHover : KDefaultButtonPressed };
	auto& Vertices{ m_Internal2D.GetVertices() };
	for (auto& Vertex : Vertices)
	{
		Vertex.ColorTex = StateColor;
	}
	m_Internal2D.UpdateBuffers();
}

// CImage
CImage::CImage(ID3D11Device* const Device, ID3D11DeviceContext* const DeviceContext, CBFNTRenderer* const BFNTRenderer) :
	CWidget(Device, DeviceContext, BFNTRenderer)
{
}

CImage::~CImage()
{
}

void CImage::Create(const std::string& Name, const SInt2& Size, const SFloat4& TexCoordRange, CWidget* const Parent)
{
	_Create(EWidgetType::Image, Name, Size, Parent);
	m_Internal2D.CreateImage(m_Size, TexCoordRange);
}

void CImage::Draw() const
{
	m_Internal2D.Draw();
}

void CImage::_SetState()
{
}

// CImageButton
CImageButton::CImageButton(ID3D11Device* const Device, ID3D11DeviceContext* const DeviceContext, CBFNTRenderer* const BFNTRenderer) :
	CWidget(Device, DeviceContext, BFNTRenderer)
{
}

CImageButton::~CImageButton()
{
}

void CImageButton::Create(const std::string& Name, const SInt2& Size, CWidget* const Parent)
{
	_Create(EWidgetType::ImageButton, Name, Size, Parent);
	m_Internal2D.CreateImage(m_Size, SFloat4(0, 0, 0, 0));
}

void CImageButton::Draw() const
{
	m_Internal2D.Draw();
	DrawCaption();
}

void CImageButton::_SetState()
{
	SFloat4 StateTexCoordRange{ (m_eState == EWidgetState::Normal) ? m_TexCoordRangeNormal :
		(m_eState == EWidgetState::Hover) ? m_TexCoordRangeHover : m_TexCoordRangePressed };

	auto& Vertices{ m_Internal2D.GetVertices() };
	Vertices[0].ColorTex = SFloat4(StateTexCoordRange.X, StateTexCoordRange.Y, 0, -1);
	Vertices[1].ColorTex = SFloat4(StateTexCoordRange.Z, StateTexCoordRange.Y, 0, -1);
	Vertices[2].ColorTex = SFloat4(StateTexCoordRange.X, StateTexCoordRange.W, 0, -1);
	Vertices[3].ColorTex = SFloat4(StateTexCoordRange.Z, StateTexCoordRange.W, 0, -1);

	m_Internal2D.UpdateBuffers();
}

CWindow::CWindow(ID3D11Device* const Device, ID3D11DeviceContext* const DeviceContext, CBFNTRenderer* const BFNTRenderer) :
	CWidget(Device, DeviceContext, BFNTRenderer)
{
}

CWindow::~CWindow()
{
}

void CWindow::Create(const std::string& Name, const SInt2& Size, const SFloat4& TexCoordRange, CWidget* const Parent)
{
	_Create(EWidgetType::Window, Name, Size, Parent);
	m_Internal2D.CreateImage(m_Size, TexCoordRange);
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

void CWindow::Draw() const
{
	m_Internal2D.Draw();
}

void CWindow::_SetState()
{
}
