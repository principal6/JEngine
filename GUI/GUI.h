#pragma once

#include "GUIBase.h"

class CGUI final : public CGUIBase
{
public:
	static constexpr const char* KSysCloseID{ "_sys_close" };
	enum class EImageButtonType
	{
		Button,
		SysButtonClose,
	};

	enum class EWindowType
	{
		Default,
	};

public:
	CGUI(ID3D11Device* const Device, ID3D11DeviceContext* const DeviceContext);
	~CGUI();

public:
	void Create(HWND hWnd);
	
public:
	bool CreateButton(const std::string& Name, const SInt2& Size, CWidget* const ParentWidget = nullptr);
	bool CreateImage(const std::string& Name, const SInt2& Size, const SInt2& U0PixelCoord, CWidget* const ParentWidget = nullptr);
	bool CreateImageButton(EImageButtonType eType, const std::string& Name, const SInt2& Size, CWidget* const ParentWidget = nullptr);
	bool CreateWindowWidget(EWindowType eType, const std::string& Name, CWidget* const ParentWidget = nullptr);

private:

};