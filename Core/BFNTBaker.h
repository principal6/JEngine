#pragma once

#include "BFNTTypes.h"

struct SCharRange
{
	SCharRange() {};
	SCharRange(wchar_t _StartCharID, wchar_t _EndCharID) : StartCharID{ _StartCharID }, EndCharID{ _EndCharID } {};

	wchar_t	StartCharID{};
	wchar_t	EndCharID{};
};

class CBFNTBaker
{
	struct SPixel32
	{
		uint8_t	R{};
		uint8_t	G{};
		uint8_t	B{};
		uint8_t	A{};
	};

	struct SImage
	{
		std::vector<SPixel32> vPixels{};
		uint32_t Width{};
		uint32_t Height{};
	};

	struct SGlyphData
	{
		SGlyphMetrics	GlyphMetrics{};
		SImage			Glyph{};
	};

public:
	CBFNTBaker();
	~CBFNTBaker();

public:
	void ClearCharRanges();
	void AddCharRange(const SCharRange& Range);

public:
	void BakeFont(const char* FontFileName, uint32_t FontSize, const char* OutDirectory);

private:
	void EstimateImageSize();

private:
	void RenderGlyphs();

private:
	void _RenderGlyphToImage(uint32_t GlyphIndex, uint32_t X, uint32_t Y);

private:
	std::vector<SCharRange> m_vCharRanges{};
	std::vector<SGlyphData>	m_vGlyphData{};
	SImage					m_Image{};

private:
	SCharRange				m_DefaultCharRange{ 0, 0xFF };
};