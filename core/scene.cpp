#include "scene.h"
#include "input/input_manager.h"

Scene::Scene()
{
	// ��ʼ�������
	m_camera = std::make_unique<Camera>(glm::vec3(12.0f, 8.0f, 5.0f), 220.0f, -18.0f, 50.0f, 0.1f);
	m_camera->setFovy(45.0f);
	m_camera->setClipping(0.1f, 1000.0f);

	// ������������¼�
	InputManager::getInstance().registerKeyPressed(std::bind(&Camera::onKeyPressed, m_camera, std::placeholders::_1));
	InputManager::getInstance().registerKeyReleased(std::bind(&Camera::onKeyReleased, m_camera, std::placeholders::_1));
	InputManager::getInstance().registerMouseOffseted(std::bind(&Camera::onMouseOffseted, m_camera, std::placeholders::_1, std::placeholders::_2));
	InputManager::getInstance().registerMousePressed(std::bind(&Camera::onMousePressed, m_camera, std::placeholders::_1));
	InputManager::getInstance().registerMouseReleased(std::bind(&Camera::onMouseReleased, m_camera, std::placeholders::_1));
}

Scene::~Scene()
{

}

void Scene::tick(float deltaTime)
{
	m_camera->tick(deltaTime);
}

void Scene::addEntity(const std::string& name, std::shared_ptr<Entity>& entity)
{
	if (m_entities.find(name) != m_entities.end())
	{
		printf("the name %s has existed already!\n", name.c_str());
	}
	m_entities[name] = entity;
}

void Scene::removeEntity(const std::string& name)
{
	if (m_entities.find(name) == m_entities.end())
	{
		printf("the name %s doesn't exist!\n", name.c_str());
	}
	m_entities.erase(name);
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