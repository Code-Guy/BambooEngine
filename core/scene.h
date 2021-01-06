#pragma once

#include "entity/entity.h"
#include "camera.h"
#include <map>
#include <memory>

class Scene
{
public:
	void init(uint32_t width, uint32_t height);
	void destroy();

	void tick(float deltaTime);

	void registerEntity(const std::string& name, std::shared_ptr<Entity> entity);
	void unregisterEntity(const std::string& name);
	std::shared_ptr<Entity> getEntity(const std::string& name);
	std::map<std::string, std::shared_ptr<Entity>>& getEntities();

	void onViewportSize(uint32_t width, uint32_t height);

private:
	std::unique_ptr<Camera> m_camera;
	std::map<std::string, std::shared_ptr<Entity>> m_entities;
};