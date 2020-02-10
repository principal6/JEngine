#pragma once

#include "BFNTTypes.h"

class CBFNTLoader
{
public:
	CBFNTLoader();
	~CBFNTLoader();

public:
	void Load(const char* BFNTFileName);

public:
	const SBFNTData& GetData() const;

private:
	SBFNTData	m_Data{};
};