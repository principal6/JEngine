#pragma once

#include "../Core/SharedHeader.h"
#include "../Model/ObjectTypes.h"

// SPatternState is created per SObjectIdentifier(Object/Instance) in CIntelligence
struct SPatternState
{
	size_t				StateID{};
	size_t				InstructionIndex{};
	long long			InstructionEndTime{}; // unit: ms
	float				WalkSpeed{ 1.0f };
	SObjectIdentifier	Me{};
	SObjectIdentifier	Enemy{};
};
