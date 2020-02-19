#pragma once

#include <cstdint>

union UUTF8_ID
{
	uint32_t	ID{};
	char		Chars[4];
};

uint32_t ConvertWideToUTF8(wchar_t Char);

// @important: don't forget to delete[] OutString
void ConvertWideToUTF8(const wchar_t* const WideString, char** OutUTF8String);

// @important: don't forget to delete[] OutString
void ConvertUTF8ToWide(const char* const UTF8String, wchar_t** OutWideString);

uint32_t GetUTF8CharacterByteCount(char FirstCharacter);
size_t GetUTF8StringLength(const char* UTF8String);
size_t ConvertStringAtToByteAt(const char* UTF8String, size_t StringAt);
bool IsEmptyString(const char* UTF8String);
