#pragma once

#include <set>
#include <map>
#include <memory>
#include <entt/entt.hpp>

#include "camera.h"
#include "timer_manager.h"

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
	
	std::shared_ptr<TimerManager> getTimerManager() { return m_timerManager; }

	void onViewportSize(uint32_t width, uint32_t height);

private:
	entt::registry& getRegistry() { return m_registry; };
	void removeEntity(const std::string& name);

	void tickTransform(float deltaTime);
	void tickAnimation(float deltaTime);

	entt::registry m_registry;

	std::shared_ptr<class Entity> m_rootEntity;
	std::map<std::string, std::shared_ptr<class Entity>> m_entities;

	std::unique_ptr<Camera> m_camera;
	std::shared_ptr<TimerManager> m_timerManager;
};