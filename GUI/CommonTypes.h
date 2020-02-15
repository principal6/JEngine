#pragma once

#include <cstdint>

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
	Window
};

enum class EEventType
{
	None, // @important
	MouseDown,
	MouseUp,
	Clicked,
	MouseMove,
	KeyStroke,
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

// Default color theme
static constexpr SFloat4 KDefaultButtonNormal{ 0, 0.4375f, 0.625f, 1 };
static constexpr SFloat4 KDefaultButtonHover{ 0, 0.625f, 0.875f, 1 };
static constexpr SFloat4 KDefaultButtonPressed{ 0, 0.75f, 0.9375f, 1 };
static constexpr float KDefaultRoundness{ 0.25f };
