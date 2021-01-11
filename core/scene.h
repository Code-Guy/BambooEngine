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
	void init(std::shared_ptr<class Renderer> renderer);
	void destroy();

	void pre();
	void begin();
	void tick(float deltaTime);
	void end();
	void post();

	friend class Entity;
	std::shared_ptr<class Entity> createEntity(const std::string& name);

	std::shared_ptr<TimerManager> getTimerManager() { return m_timerManager; }

private:
	entt::registry& getRegistry() { return m_registry; };
	void removeEntity(const std::string& name);

	void tickTransform(float deltaTime);
	void tickEvent(float deltaTime);
	void tickAnimation(float deltaTime);

	entt::registry m_registry;

	std::shared_ptr<class Renderer> m_renderer;
	std::shared_ptr<class Entity> m_rootEntity;
	std::map<std::string, std::shared_ptr<class Entity>> m_entities;

	std::unique_ptr<Camera> m_camera;
	std::shared_ptr<TimerManager> m_timerManager;
};