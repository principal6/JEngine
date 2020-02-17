#include "GUI.h"

using std::string;

CGUI::CGUI(ID3D11Device* const Device, ID3D11DeviceContext* const DeviceContext) : CGUIBase(Device, DeviceContext)
{
}

CGUI::~CGUI()
{
}

void CGUI::Create(HWND hWnd)
{
	_Create(hWnd, "Asset\\D2Coding_16.bfnt");
	_CreateTextureAtlas("Asset\\GUI.png");
}

bool CGUI::CreateButton(const std::string& Name, const SInt2& Size, CWidget* const ParentWidget)
{
	return _CreateButton(Name, Size, ParentWidget);
}

bool CGUI::CreateImage(const std::string& Name, const SInt2& Size, const SInt2& U0PixelCoord, CWidget* const ParentWidget)
{
	return _CreateImage(Name, Size, U0PixelCoord, ParentWidget);
}

bool CGUI::CreateImageButton(EImageButtonType eType, const std::string& Name, const SInt2& Size, CWidget* const ParentWidget)
{
	if (_CreateImageButton(Name, Size, ParentWidget))
	{
		auto Widget{ GetWidget(Name, ParentWidget) };
		auto WidgetSize{ Widget->GetSize() };

		string FullName{ Widget->GetFullName() };
		switch (eType)
		{
		case CGUI::EImageButtonType::Button:
			SetWidgetStateTexCoordRange(FullName, EWidgetState::Normal , SInt2(0, 96 + 72 * 0), WidgetSize);
			SetWidgetStateTexCoordRange(FullName, EWidgetState::Hover  , SInt2(0, 96 + 72 * 2), WidgetSize);
			SetWidgetStateTexCoordRange(FullName, EWidgetState::Pressed, SInt2(0, 96 + 72 * 1), WidgetSize);
			break;
		case CGUI::EImageButtonType::SysButtonClose:
			SetWidgetStateTexCoordRange(FullName, EWidgetState::Normal , SInt2(480 + 48 * 0, 480 + 48), WidgetSize);
			SetWidgetStateTexCoordRange(FullName, EWidgetState::Hover  , SInt2(480 + 48 * 1, 480 + 48), WidgetSize);
			SetWidgetStateTexCoordRange(FullName, EWidgetState::Pressed, SInt2(480 + 48 * 2, 480 + 48), WidgetSize);
			break;
		default:
			break;
		}
		return true;
	}
	return false;
}

bool CGUI::CreateWindowWidget(EWindowType eType, const std::string& Name, CWidget* const ParentWidget)
{
	switch (eType)
	{
	case CGUI::EWindowType::Default:
		return _CreateWindowPrimitive(Name, SInt2(396, 294), KDefaultRoundness, ParentWidget);

		//if (_CreateWindowImage(Name, SInt2(396, 294), SInt2(0, 480), SInt2(396, 294), SInt2(5, 0), SInt2(318, 31), ParentWidget))
		//{
		//	CWidget* const Window{ GetWidget(Name, ParentWidget) };
		//	{
		//		assert(CreateImageButton(CGUI::EImageButtonType::SysButtonClose, KSysCloseID, SInt2(48, 48), Window));
		//		CWidget* Widget{ GetWidget(KSysCloseID, Window) };
		//		Widget->SetOffset(SInt2(350, -8));
		//		Widget->SetSelectionSize(SInt2(36, 36));
		//	}
		//	// todo: focus change
		//	return true;
		//}
		
		break;
	default:
		break;
	}
	return false;
}

bool CGUI::CreateText(const std::string& Name, const SInt2& Size, const std::string& Content, CWidget* const ParentWidget)
{
	return _CreateText(Name, Size, Content, ParentWidget);
}

bool CGUI::CreateTextEdit(const std::string& Name, const SInt2& Size, CWidget* const ParentWidget)
{
	return _CreateTextEdit(Name, Size, ParentWidget);
}
