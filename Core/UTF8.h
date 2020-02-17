#pragma once

#include <cstdint>

union UUTF8_ID
{
	uint32_t	ID{};
	char		Chars[4];
};

uint32_t ConvertToUTF8(wchar_t Char);
uint32_t GetUTF8CharacterByteCount(char FirstCharacter);
size_t GetUTF8StringLength(const char* UTF8String);
size_t ConvertStringAtToByteAt(const char* UTF8String, size_t StringAt);
bool IsEmptyString(const char* UTF8String);
