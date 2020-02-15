#include "UTF8.h"

#include <Windows.h>
#include <cassert>

uint32_t ConvertToUTF8(wchar_t Char)
{
	UUTF8_ID Result{};
	WideCharToMultiByte(CP_UTF8, 0, &Char, 1, Result.Chars, 4, nullptr, nullptr);
	return Result.ID;
}

uint32_t GetUTF8CharacterByteCount(char FirstCharacter)
{
	uint8_t Cmp{ static_cast<uint8_t>(FirstCharacter) };
	
	// Non-ASCII
	if (Cmp >= 0b11110000) return 4;
	if (Cmp >= 0b11100000) return 3;
	if (Cmp >= 0b11000000) return 2;

	// ASCII
	if (Cmp <= 0b01111111) return 1;

	return 0;
}

size_t GetUTF8StringLength(const char* UTF8String)
{
	assert(UTF8String);

	size_t Length{};
	size_t At{};
	size_t StringSize{ strlen(UTF8String) };
	while (At < StringSize)
	{
		size_t ByteCount{ GetUTF8CharacterByteCount(UTF8String[At]) };
		At += ByteCount;

		++Length;
	}
	
	return Length;
}

bool IsEmptyString(const char* UTF8String)
{
	return (strlen(UTF8String) == 0) ? true : false;
}
