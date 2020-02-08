#pragma once

#include "Core/SharedHeader.h"

class CObject3D;
class CPattern;

enum class ESpawningCondition
{
	OnlyOnce,
	OnceInAWhile,
	NTimes,
	NMaintained,
};

static constexpr size_t KSpawningCountMaxLimit{ 1000 };
struct SMonsterSpawnerData
{
	std::string			Object3DName{};
	std::string			PatternFileName{};

	DirectX::XMFLOAT3	Size{};
	ESpawningCondition	eCondition{};
	float				Interval{ 1000.0f };
	size_t				CountMax{};
};

class CMonsterSpawner
{
public:
	CMonsterSpawner(const std::string& Name, const SMonsterSpawnerData& Data);
	~CMonsterSpawner();

public:
	void Reset();
	void SetData(const SMonsterSpawnerData& Data);

public:
	bool IsInitialized() const;
	bool Spawn() const;

public:
	const std::string& GetName() const;
	const SMonsterSpawnerData& GetData() const;
	size_t GetSpawningCount() const;

private:
	std::string			m_Name{};
	SMonsterSpawnerData	m_Data{};

private:
	mutable size_t		m_SpawningCounter{};
	mutable long long	m_PrevSpawningTime{};
	mutable bool		m_bInitialized{ false };
};