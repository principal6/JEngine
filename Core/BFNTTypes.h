#pragma once

#include <string>
#include <vector>
#include <unordered_map>

struct SGlyphMetrics
{
	uint32_t	ID_UTF8{};
	uint32_t	Width{};
	uint32_t	Height{};
	uint32_t	XAdvance{};

	int32_t		XBearing{};
	int32_t		YBearing{};

	float		U0{};
	float		V0{};

	float		U1{};
	float		V1{};
};

struct SBFNTData
{
	std::string								FamilyName{};
	uint32_t								FontSize{};
	uint32_t								LineHeight{};
	uint32_t								TextureWidth{};
	uint32_t								TextureHeight{};
	std::vector<SGlyphMetrics>				vGlyphs{};
	std::unordered_map<uint32_t, size_t>	umapGlyphIDtoIndex{};
};