#include "BFNTLoader.h"
#include "BinaryData.h"
#include "UTF8.h"

CBFNTLoader::CBFNTLoader()
{
}

CBFNTLoader::~CBFNTLoader()
{
}

void CBFNTLoader::Load(const char* BFNTFileName)
{
	CBinaryData BinaryData{};
	BinaryData.LoadFromFile(BFNTFileName);
	
	BinaryData.ReadSkip(8); // KJW_BFNT

	BinaryData.ReadStringWithPrefixedLength(m_Data.FamilyName);
	BinaryData.ReadUint32(m_Data.FontSize);
	BinaryData.ReadUint32(m_Data.LineHeight);
	BinaryData.ReadUint32(m_Data.TextureWidth);
	BinaryData.ReadUint32(m_Data.TextureHeight);

	uint32_t GlyphCount{ BinaryData.ReadUint32() };
	m_Data.vGlyphs.resize(GlyphCount);
	m_Data.umapGlyphIDtoIndex.clear();
	const float KWidthDenominator{ 1.0f / m_Data.TextureWidth };
	const float KHeightDenominator{ 1.0f / m_Data.TextureHeight };
	for (uint32_t iGlyph = 0; iGlyph < GlyphCount; ++iGlyph)
	{
		auto& Glyph{ m_Data.vGlyphs[iGlyph] };
		BinaryData.ReadUint32(Glyph.ID_UTF8);
		BinaryData.ReadUint32(Glyph.Width);
		BinaryData.ReadUint32(Glyph.Height);
		BinaryData.ReadUint32(Glyph.XAdvance);

		BinaryData.ReadInt32(Glyph.XBearing);
		BinaryData.ReadInt32(Glyph.YBearing);

		BinaryData.ReadFloat(Glyph.U0);
		BinaryData.ReadFloat(Glyph.V0);
		
		Glyph.U1 = Glyph.U0 + (Glyph.Width * KWidthDenominator);
		Glyph.V1 = Glyph.V0 + (Glyph.Height * KHeightDenominator);

		m_Data.umapGlyphIDtoIndex[Glyph.ID_UTF8] = iGlyph;
	}
}

const SBFNTData& CBFNTLoader::GetData() const
{
	return m_Data;
}
