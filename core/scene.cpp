#include "scene.h"
#include "input/input_manager.h"

void Scene::init(uint32_t width, uint32_t height)
{
	// 初始化摄像机
	m_camera = std::make_unique<Camera>(glm::vec3(12.0f, 8.0f, 5.0f), 220.0f, -18.0f, 50.0f, 0.1f);
	m_camera->setFovy(45.0f);
	m_camera->setClipping(0.1f, 1000.0f);
	m_camera->setAspect((float)width / height);

	// 绑定摄像机输入事件
	InputManager::getInstance().registerKeyPressed(std::bind(&Camera::onKeyPressed, m_camera.get(), std::placeholders::_1));
	InputManager::getInstance().registerKeyReleased(std::bind(&Camera::onKeyReleased, m_camera.get(), std::placeholders::_1));
	InputManager::getInstance().registerMouseOffseted(std::bind(&Camera::onMouseOffseted, m_camera.get(), std::placeholders::_1, std::placeholders::_2));
	InputManager::getInstance().registerMousePressed(std::bind(&Camera::onMousePressed, m_camera.get(), std::placeholders::_1));
	InputManager::getInstance().registerMouseReleased(std::bind(&Camera::onMouseReleased, m_camera.get(), std::placeholders::_1));
}

void Scene::destroy()
{
	for (const auto& iter : m_entities)
	{
		iter.second->post();
	}
	m_entities.clear();
}

void Scene::tick(float deltaTime)
{
	m_camera->tick(deltaTime);

	for (const auto& iter : m_entities)
	{
		iter.second->tick(deltaTime);
	}
}

void Scene::registerEntity(const std::string& name, std::shared_ptr<Entity> entity)
{
	if (m_entities.find(name) != m_entities.end())
	{
		printf("the name %s has existed already!\n", name.c_str());
	}
	m_entities[name] = entity;
	m_entities[name]->pre();
}

void Scene::unregisterEntity(const std::string& name)
{
	auto iter = m_entities.find(name);
	if (iter == m_entities.end())
	{
		printf("the name %s doesn't exist!\n", name.c_str());
	}

	iter->second->post();
	m_entities.erase(iter);
}

std::shared_ptr<Entity> Scene::getEntity(const std::string& name)
{
	if (m_entities.find(name) == m_entities.end())
	{
		printf("the name %s doesn't exist!\n", name.c_str());
		return nullptr;
	}
	return m_entities[name];
}

std::map<std::string, std::shared_ptr<Entity>>& Scene::getEntities()
{
	return m_entities;
}

void Scene::onViewportSize(uint32_t width, uint32_t height)
{
	m_camera->setAspect((float)width / height);
}