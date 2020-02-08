#include "MonsterSpawner.h"
#include <chrono>

using std::chrono::steady_clock;

CMonsterSpawner::CMonsterSpawner(const std::string& Name, const SMonsterSpawnerData& Data)
{
	m_Name = Name;
	SetData(Data);
}

CMonsterSpawner::~CMonsterSpawner()
{
}

void CMonsterSpawner::Reset()
{
	m_bInitialized = false;

	m_SpawningCounter = 0;
	m_PrevSpawningTime = 0;
}

void CMonsterSpawner::SetData(const SMonsterSpawnerData& Data)
{
	m_Data = Data;

	Reset();
}

bool CMonsterSpawner::IsInitialized() const
{
	if (!m_bInitialized)
	{
		m_bInitialized = true;
		return false;
	}
	return true;
}

bool CMonsterSpawner::Spawn() const
{
	static const steady_clock Clock{};
	long long Now_ms{ Clock.now().time_since_epoch().count() / 1'000'000 };

	if (m_PrevSpawningTime == 0) m_PrevSpawningTime = Now_ms;

	switch (m_Data.eCondition)
	{
	default:
	case ESpawningCondition::OnlyOnce:
		if (m_SpawningCounter == 0)
		{
			++m_SpawningCounter;
			return true;
		}
		return false;
	case ESpawningCondition::OnceInAWhile:
		if (Now_ms > m_PrevSpawningTime + m_Data.Interval)
		{
			++m_SpawningCounter;
			m_PrevSpawningTime = Now_ms;
			return true;
		}
		return false;
	case ESpawningCondition::NTimes:
		if (m_SpawningCounter < m_Data.CountMax
			&&
			Now_ms > m_PrevSpawningTime + m_Data.Interval)
		{
			++m_SpawningCounter;
			m_PrevSpawningTime = Now_ms;
			return true;
		}
		return false;
	case ESpawningCondition::NMaintained:
		if (m_SpawningCounter < m_Data.CountMax)
		{
			++m_SpawningCounter;
			return true;
		}
		return false;
	}

	return false;
}

const std::string& CMonsterSpawner::GetName() const
{
	return m_Name;
}

const SMonsterSpawnerData& CMonsterSpawner::GetData() const
{
	return m_Data;
}

size_t CMonsterSpawner::GetSpawningCount() const
{
	return m_SpawningCounter;
}
