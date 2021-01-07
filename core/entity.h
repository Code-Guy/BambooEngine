#pragma once

#include <entt/entt.hpp>

class Entity
{
public:
	Entity(entt::registry& registry, entt::entity handle);
	virtual ~Entity();

	template<typename T, typename... Args>
	T& addComponent(Args&&... args)
	{
		return m_registry.emplace<T>(m_handle, std::forward<Args>(args)...);
	}

	template<typename T>
	T& getComponent()
	{
		return m_registry.get<T>(m_handle);
	}

	template<typename T>
	bool hasComponent()
	{
		return m_registry.has<T>(m_handle);
	}

	template<typename T>
	bool removeComponent()
	{
		return m_registry.remove<T>(m_handle);
	}

private:
	entt::registry& m_registry;
	entt::entity m_handle;
};