#pragma once

#include <cstdint>

enum class EButtonPreset
{
	Close,
};

enum class EHorzAlign
{
	Left,
	Center,
	Right
};

enum class EVertAlign
{
	Top,
	Middle,
	Bottom
};

enum class EWidgetState
{
	Normal,
	Hover,
	Pressed,
};

enum class EWidgetType
{
	Invalid,
	Button,
	Image,
	ImageButton,
	Window,
	Text,
	TextEdit,
};

enum class EEventType
{
	None, // @important
	MouseDown,
	MouseUp,
	Clicked,
	MouseMove,
	KeyStroke,
	KeyDown,
	IMEComposition,
};

enum class EMouseEventType
{
	LeftButton,
	RightButton,
	MiddleButton,
	ExtraButton,
};

struct alignas(8) SInt2
{
	SInt2() = default;
	SInt2(int32_t _X, int32_t _Y) : X{ _X }, Y{ _Y } {};
	SInt2(const SInt2& b) = default;
	SInt2& operator=(const SInt2& b)
	{
		X = b.X;
		Y = b.Y;
		return *this;
	}
	SInt2 operator+(const SInt2& b) const
	{
		return SInt2(X + b.X, Y + b.Y);
	}
	SInt2 operator-(const SInt2& b) const
	{
		return SInt2(X - b.X, Y - b.Y);
	}

	int32_t X{};
	int32_t Y{};
};

struct alignas(8) SFloat2
{
	SFloat2() = default;
	SFloat2(float _X, float _Y) : X{ _X }, Y{ _Y } {};
	SFloat2(const SFloat2& b) = default;
	SFloat2& operator=(const SFloat2& b)
	{
		X = b.X;
		Y = b.Y;
		return *this;
	}
	
	float X{};
	float Y{};
};

struct alignas(16) SFloat4
{
	SFloat4() = default;
	constexpr SFloat4(float _X, float _Y, float _Z, float _W) : X{ _X }, Y{ _Y }, Z{ _Z }, W{ _W } {};
	SFloat4(const SFloat4& b) = default;
	SFloat4& operator=(const SFloat4 &b)
	{
		X = b.X;
		Y = b.Y;
		Z = b.Z;
		W = b.W;
		return *this;
	}
	SFloat4 operator+(const SFloat4& b) const
	{
		return SFloat4(X + b.X, Y + b.Y, Z + b.Z, W + b.W);
	}
	SFloat4 operator-(const SFloat4& b) const
	{
		return SFloat4(X - b.X, Y - b.Y, Z - b.Z, W - b.W);
	}

	float X{};
	float Y{};
	float Z{};
	float W{};
};

struct SVertex
{
	SVertex() {}
	SVertex(const SFloat4& _Position, const SFloat4& _ColorTex) : Position{ _Position }, ColorTex{ _ColorTex } {}

	SFloat4 Position{};
	SFloat4 ColorTex{};
};

struct ID3D11Device;
struct ID3D11DeviceContext;
class CBFNTRenderer;
class CShader;
class CConstantBuffer;
struct SWidgetCtorData
{
	SWidgetCtorData() {};
	SWidgetCtorData(
		ID3D11Device* const _Device, ID3D11DeviceContext* const _DeviceContext,
		CBFNTRenderer* const _BFNTRenderer, CShader* const _VS, CShader* const _PS,
		CConstantBuffer* const _CBSpace, void* const _CBSpaceData, SFloat2* const _WindowSize) :
		Device{ _Device }, DeviceContext{ _DeviceContext },
		BFNTRenderer{ _BFNTRenderer }, VS{ _VS }, PS{ _PS },
		CBSpace{ _CBSpace }, CBSpaceData{ _CBSpaceData }, WindowSize{ _WindowSize } {};

	ID3D11Device*			Device{};
	ID3D11DeviceContext*	DeviceContext{};
	CBFNTRenderer*			BFNTRenderer{};
	CShader*				VS{};
	CShader*				PS{};
	CConstantBuffer*		CBSpace{};
	void*					CBSpaceData{};
	SFloat2*				WindowSize{};
};

// Default color theme
static constexpr SFloat4 KDefaultButtonNormalColor{ 0, 0.4375f, 0.625f, 1 };
static constexpr SFloat4 KDefaultButtonHoverColor{ 0, 0.625f, 0.875f, 1 };
static constexpr SFloat4 KDefaultButtonPressedColor{ 0, 0.75f, 0.9375f, 1 };
static constexpr SFloat4 KDefaultTitleBarActiveColor{ 0, 0.3125f, 0.625f, 1 };
static constexpr SFloat4 KDefaultTitleBarInactiveColor{ 0.25f, 0.5f, 0.75f, 1 };
static constexpr SFloat4 KDefaultWindowBackgroundColor{ 0.125f, 0.1875f, 0.25f, 1 };
static constexpr SFloat4 KDefaultTextEditBackgroundColor{ 0.0625f, 0.125f, 0.1875f, 1 };
static constexpr SFloat4 KDefaultIconColor{ 0.875f, 1.0f, 1.0f, 1 };
static constexpr SFloat4 KDefaultFontColor{ KDefaultIconColor };
static constexpr SFloat4 KDefaultCaretColor{ 1.0f, 1.0f, 1.0f, 1 };
static constexpr SFloat4 KDefaultSelectionColor{ 0.5f, 0.75f, 1.0f, 0.5f };
//static constexpr float KDefaultRoundness{ 0.25f }; // relative roundness -unit: percentage
static constexpr float KDefaultRoundness{ 6 }; // absolute roundness -unit: pixel
static constexpr int32_t KDefaultTitleBarHeight{ 24 };
static constexpr int32_t KDefaultTitleBarCaptionIndent{ 6 };