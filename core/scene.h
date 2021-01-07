#pragma once

#include "entity.h"
#include "camera.h"
#include <map>

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

	Entity createEntity(const std::string& name);

	void onViewportSize(uint32_t width, uint32_t height);

private:
	Entity m_rootEntity;
	std::unique_ptr<Camera> m_camera;

	std::shared_ptr<entt::registry> m_registry;
};