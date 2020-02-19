#pragma comment(lib, "freetype_x64_debug.lib")

#include "BFNTBaker.h"
#include "../FreeType/ft2build.h"
#include FT_FREETYPE_H

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb/stb_image_write.h"

#include "UTF8.h"
#include "BinaryData.h"

#include <string>
#include <algorithm>

using std::string;
using std::vector;
using std::to_string;
using std::sort;
using std::min;

CBFNTBaker::CBFNTBaker()
{
}

CBFNTBaker::~CBFNTBaker()
{
}

void CBFNTBaker::ClearCharRanges()
{
	m_vCharRanges.clear();
}

void CBFNTBaker::AddCharRange(const SCharRange& Range)
{
	m_vCharRanges.emplace_back(Range);
}

void CBFNTBaker::BakeFont(const char* FontFileName, uint32_t FontSize, const char* OutDirectory)
{
	string _FontFileName{ FontFileName };
	for (auto& ch : _FontFileName)
	{
		if (ch == '/') ch = '\\';
	}

	string Dir{ _FontFileName };
	string FileNameOnly{ _FontFileName };
	{
		size_t At{ Dir.find_last_of('\\') };
		if (At != string::npos)
		{
			Dir = Dir.substr(0, At + 1);
		}
		else
		{
			Dir.clear();
		}

		FileNameOnly = FileNameOnly.substr(Dir.size());

		size_t Ext{ FileNameOnly.find_last_of('.') };
		if (Ext != string::npos)
		{
			FileNameOnly = FileNameOnly.substr(0, Ext);
		}
	}

	string _OutDirectory{ OutDirectory };
	if (_OutDirectory.size())
	{
		for (auto& ch : _OutDirectory)
		{
			if (ch == '/') ch = '\\';
		}
		if (_OutDirectory.back() != '\\') _OutDirectory += '\\';
	}

	FT_Library ftLibrary{};
	FT_Face ftFace{};

	assert(FT_Init_FreeType(&ftLibrary) == FT_Err_Ok);
	assert(FT_New_Face(ftLibrary, _FontFileName.c_str(), 0, &ftFace) == FT_Err_Ok);

	// 72 dpi (horz & vert)
	assert(FT_Set_Char_Size(ftFace, FontSize << 6, 0, 72, 0) == FT_Err_Ok);

	// @important: wchar_t
	assert(FT_Select_Charmap(ftFace, FT_ENCODING_UNICODE) == FT_Err_Ok);
	
	if (m_vCharRanges.empty()) AddCharRange(m_DefaultCharRange);

	for (const auto& CharRange : m_vCharRanges)
	{
		for (wchar_t wch = CharRange.StartCharID; wch <= CharRange.EndCharID; ++wch)
		{
			assert(FT_Load_Char(ftFace, wch, FT_LOAD_RENDER) == FT_Err_Ok);

			m_vGlyphData.emplace_back();

			auto& GlyphMetrics{ m_vGlyphData.back().GlyphMetrics };
			auto& Glyph{ m_vGlyphData.back().Glyph };

			GlyphMetrics.ID_UTF8 = ConvertWideToUTF8(wch);
			GlyphMetrics.Width = ftFace->glyph->metrics.width >> 6;
			GlyphMetrics.Height = ftFace->glyph->metrics.height >> 6;
			GlyphMetrics.XAdvance = ftFace->glyph->metrics.horiAdvance >> 6;
			GlyphMetrics.XBearing = ftFace->glyph->metrics.horiBearingX >> 6;
			GlyphMetrics.YBearing = ftFace->glyph->metrics.horiBearingY >> 6;

			// @vertical metrics are not used
			//ftFace->glyph->metrics.vertAdvance;
			//ftFace->glyph->metrics.vertBearingX;
			//ftFace->glyph->metrics.vertBearingY;

			uint32_t PixelCount{ GlyphMetrics.Width * GlyphMetrics.Height };
			Glyph.vPixels.resize(PixelCount);
			for (uint32_t iPixel = 0; iPixel < PixelCount; ++iPixel)
			{
				// Gray to RGBA
				uint8_t SourceAlpha{ ftFace->glyph->bitmap.buffer[iPixel] };
				Glyph.vPixels[iPixel].A = SourceAlpha;
			}
		}
	}

	EstimateImageSize();
	RenderGlyphs();

	string FontSizeSuffix{ "_" + to_string(FontSize) };

	string TextureFileName{ _OutDirectory + FileNameOnly + FontSizeSuffix + ".png" };
	stbi_write_png(TextureFileName.c_str(), m_Image.Width, m_Image.Height, sizeof(SPixel32), &m_Image.vPixels[0], m_Image.Width * sizeof(SPixel32));

	string DataFileName{ _OutDirectory + FileNameOnly + FontSizeSuffix + ".bfnt" };
	CBinaryData BinaryData{};
	BinaryData.WriteString("KJW_BFNT", 8);
	BinaryData.WriteStringWithPrefixedLength(ftFace->family_name);
	BinaryData.WriteUint32(FontSize);
	BinaryData.WriteUint32((ftFace->ascender - ftFace->descender) >> 6); // line height
	BinaryData.WriteInt32(ftFace->underline_position >> 6); // underline position
	BinaryData.WriteUint32(m_Image.Width);
	BinaryData.WriteUint32(m_Image.Height);
	BinaryData.WriteUint32((uint32_t)m_vGlyphData.size());
	for (const auto& GlyphData : m_vGlyphData)
	{
		const auto& GlyphMetrics{ GlyphData.GlyphMetrics };
		BinaryData.WriteUint32(GlyphMetrics.ID_UTF8);
		BinaryData.WriteUint32(GlyphMetrics.Width);
		BinaryData.WriteUint32(GlyphMetrics.Height);
		BinaryData.WriteUint32(GlyphMetrics.XAdvance);

		BinaryData.WriteInt32(GlyphMetrics.XBearing);
		BinaryData.WriteInt32(GlyphMetrics.YBearing);

		BinaryData.WriteFloat(GlyphMetrics.U0);
		BinaryData.WriteFloat(GlyphMetrics.V0);
	}
	BinaryData.SaveToFile(DataFileName);

	FT_Done_Face(ftFace);
	FT_Done_FreeType(ftLibrary);
}

void CBFNTBaker::EstimateImageSize()
{
	sort(m_vGlyphData.begin(), m_vGlyphData.end(), [](const SGlyphData& a, const SGlyphData& b) { return a.GlyphMetrics.Height < b.GlyphMetrics.Height; });

	m_Image.vPixels.clear();
	m_Image.Width = 2048;
	m_Image.Height = 128;

	uint32_t CursorX{};
	uint32_t CursorY{};
	uint32_t LineHeight{};
	constexpr uint32_t IntervalX{ 1 };
	constexpr uint32_t IntervalY{ 1 };

	for (uint32_t iGlyph = 0; iGlyph < (uint32_t)m_vGlyphData.size(); ++iGlyph)
	{
		const auto& GlyphMetrics{ m_vGlyphData[iGlyph].GlyphMetrics };

		if (GlyphMetrics.Height > LineHeight) LineHeight = GlyphMetrics.Height;

		if (CursorX + GlyphMetrics.Width + IntervalX >= m_Image.Width)
		{
			CursorX = 0;
			CursorY += LineHeight + IntervalY;
			LineHeight = 0;

			if (CursorY >= m_Image.Height)
			{
				m_Image.Height *= 2;
			}
		}

		CursorX += GlyphMetrics.Width + IntervalX;
	}

	m_Image.vPixels.resize(m_Image.Width * m_Image.Height);
}

void CBFNTBaker::RenderGlyphs()
{
	uint32_t CursorX{};
	uint32_t CursorY{};
	uint32_t LineHeight{};
	constexpr uint32_t IntervalX{ 1 };
	constexpr uint32_t IntervalY{ 1 };

	for (uint32_t iGlyph = 0; iGlyph < (uint32_t)m_vGlyphData.size(); ++iGlyph)
	{
		const auto& GlyphMetrics{ m_vGlyphData[iGlyph].GlyphMetrics };

		if (GlyphMetrics.Height > LineHeight) LineHeight = GlyphMetrics.Height;

		if (CursorX + GlyphMetrics.Width + IntervalX >= m_Image.Width)
		{
			CursorX = 0;
			CursorY += LineHeight + IntervalY;
			LineHeight = 0;
		}

		_RenderGlyphToImage(iGlyph, CursorX, CursorY);

		CursorX += GlyphMetrics.Width + IntervalX;
	}
}

void CBFNTBaker::_RenderGlyphToImage(uint32_t GlyphIndex, uint32_t X, uint32_t Y)
{
	auto& GlyphData{ m_vGlyphData[GlyphIndex] };
	auto& GlyphMetrics{ GlyphData.GlyphMetrics };
	GlyphMetrics.U0 = (float)X / (float)m_Image.Width;
	GlyphMetrics.V0 = (float)Y / (float)m_Image.Height;

	for (uint32_t _y = 0; _y < GlyphMetrics.Height; ++_y)
	{
		for (uint32_t _x = 0; _x < GlyphMetrics.Width; ++_x)
		{
			uint32_t ImageIndex{ (Y + _y) * m_Image.Width + X + _x };
			if (ImageIndex >= m_Image.vPixels.size()) return; // no more space
			
			m_Image.vPixels[ImageIndex] = GlyphData.Glyph.vPixels[_y * GlyphMetrics.Width + _x];
		}
	}
}
