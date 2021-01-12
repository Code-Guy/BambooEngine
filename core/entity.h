#pragma once

#include "scene.h"

class Entity
{
public:
	Entity(Scene* scene, entt::entity handle);
	~Entity();

	void attach(std::shared_ptr<Entity> parent);
	void detach();
	void destroy();

	void update();

	template<typename T, typename... Args>
	T& addComponent(Args&&... args)
	{
		return m_scene->getRegistry().emplace<T>(m_handle, std::forward<Args>(args)...);
	}

	template<typename T>
	T& cloneComponent(std::shared_ptr<Entity> other)
	{
		return m_scene->getRegistry().emplace<T>(m_handle, other->getComponent<T>());
	}

	template<typename T>
	T& getComponent()
	{
		return m_scene->getRegistry().get<T>(m_handle);
	}

	template<typename T>
	bool hasComponent()
	{
		return m_scene->getRegistry().has<T>(m_handle);
	}

	template<typename T>
	bool removeComponent()
	{
		return m_scene->getRegistry().remove<T>(m_handle);
	}

private:
	Scene* m_scene;
	entt::entity m_handle;

	Entity* m_parent;
	std::set<Entity*> m_children;
};