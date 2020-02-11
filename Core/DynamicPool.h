#pragma once

#include <vector>
#include <string>
#include <unordered_map>

template <typename T>
class CDynamicPool
{
public:
	CDynamicPool() {};
	CDynamicPool(const CDynamicPool& b) = delete;
	~CDynamicPool() { Clear(); };

public:
	bool Exists(const std::string& Name) const
	{
		return (m_umapObjectNameToIndex.find(Name) != m_umapObjectNameToIndex.end());
	}

	bool Insert(const std::string& Name, T*& New)
	{
		if (m_umapObjectNameToIndex.find(Name) != m_umapObjectNameToIndex.end())
		{
			delete New;
			New = nullptr;

			return false;
		}

		m_vObjects.emplace_back(New);

		m_vObjectNames.emplace_back(Name);

		m_umapObjectNameToIndex[Name] = m_vObjects.size() - 1;

		return true;
	}

	void Delete(const std::string& Name)
	{
		std::string DelName{ Name };
		if (m_umapObjectNameToIndex.find(DelName) != m_umapObjectNameToIndex.end())
		{
			size_t DelAt{ m_umapObjectNameToIndex.at(DelName) };
			if (DelAt < m_vObjects.size() - 1)
			{
				std::swap(m_umapObjectNameToIndex[DelName], m_umapObjectNameToIndex[m_vObjectNames.back()]);

				std::swap(m_vObjectNames[DelAt], m_vObjectNames.back());

				std::swap(m_vObjects[DelAt], m_vObjects.back());
			}

			m_umapObjectNameToIndex.erase(DelName);

			m_vObjectNames.pop_back();

			delete m_vObjects.back();
			m_vObjects.back() = nullptr;

			m_vObjects.pop_back();
		}
	}

	void Clear()
	{
		m_umapObjectNameToIndex.clear();

		m_vObjectNames.clear();

		for (auto& Object : m_vObjects)
		{
			delete Object;
			Object = nullptr;
		}
		m_vObjects.clear();
	}

	T* Get(const std::string& Name) const
	{
		if (m_umapObjectNameToIndex.find(Name) != m_umapObjectNameToIndex.end())
		{
			return m_vObjects[m_umapObjectNameToIndex.at(Name)];
		}
		return nullptr;
	}

	size_t Count() const
	{
		return m_vObjects.size();
	}

	const std::string& GetName(size_t Index) const
	{
		return m_vObjectNames[Index];
	}

	const std::vector<std::string>& GetNames() const
	{
		return m_vObjectNames;
	}

private:
	std::vector<T*>							m_vObjects{};
	std::vector<std::string>				m_vObjectNames{};
	std::unordered_map<std::string, size_t>	m_umapObjectNameToIndex{};
};
