#include "UTF8.h"

#include <Windows.h>
#include <cassert>

uint32_t ConvertWideToUTF8(wchar_t Char)
{
	UUTF8_ID Result{};
	WideCharToMultiByte(CP_UTF8, 0, &Char, 1, Result.Chars, 4, nullptr, nullptr);
	return Result.ID;
}

void ConvertWideToUTF8(const wchar_t* const WideString, char** OutUTF8String)
{
	int ByteCount{ WideCharToMultiByte(CP_UTF8, 0, WideString, (int)wcslen(WideString), nullptr, 0, 0, 0) };
	*OutUTF8String = new char[(size_t)ByteCount + 1] {};
	WideCharToMultiByte(CP_UTF8, 0, WideString, (int)wcslen(WideString), *OutUTF8String, ByteCount, 0, 0);
}

void ConvertUTF8ToWide(const char* const UTF8String, wchar_t** OutWideString)
{
	int Length{ MultiByteToWideChar(CP_UTF8, 0, UTF8String, (int)strlen(UTF8String), nullptr, 0) };
	*OutWideString = new wchar_t[(size_t)Length + 1]{};
	MultiByteToWideChar(CP_UTF8, 0, UTF8String, (int)strlen(UTF8String), *OutWideString, Length);
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

size_t ConvertStringAtToByteAt(const char* UTF8String, size_t StringAt)
{
	assert(UTF8String);

	if (StringAt == 0) return 0;

	size_t At{};
	size_t ByteAt{};
	size_t ByteSize{ strlen(UTF8String) };
	while (ByteAt < ByteSize)
	{
		size_t ByteCount{ GetUTF8CharacterByteCount(UTF8String[ByteAt]) };
		ByteAt += ByteCount;

		++At;
		if (StringAt == At) break;
	}
	return ByteAt;
}

bool IsEmptyString(const char* UTF8String)
{
	return (strlen(UTF8String) == 0) ? true : false;
}
