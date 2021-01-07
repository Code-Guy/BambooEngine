#pragma once

#include <set>
#include <map>
#include <memory>
#include <chrono>
#include <entt/entt.hpp>
#include "camera.h"

class Scene
{
public:
	void init(uint32_t width, uint32_t height);
	void destroy();

	void pre();
	void begin();
	void tick(float deltaTime);
	void end();
	void post();

	friend class Entity;
	std::shared_ptr<class Entity> createEntity(const std::string& name);
	
	void onViewportSize(uint32_t width, uint32_t height);
	float time();

private:
	entt::registry& getRegistry() { return m_registry; };
	void removeEntity(const std::string& name);

	entt::registry m_registry;

	std::shared_ptr<class Entity> m_rootEntity;
	std::map<std::string, std::shared_ptr<class Entity>> m_entities;

	std::unique_ptr<Camera> m_camera;

	std::chrono::steady_clock::time_point m_beginTime;
};