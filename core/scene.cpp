#include "scene.h"
#include "entity.h"
#include "input/input_manager.h"
#include "component/component.h"
#include "io/asset_loader.h"
#include "utility/utility.h"

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

	// 添加根节点
	m_rootEntity = createEntity("root");

	// 加载模型资源，生成组件
	std::vector<std::string> modelNames = {
		//"asset/model/ground/ground.fbx",
		//"asset/model/dinosaur/dinosaur.fbx",
		//"asset/model/armadillo/armadillo.fbx",
		"asset/model/dragon/dragon.fbx",
		"asset/model/ogre/ogre.fbx",
		"asset/model/sponza/sponza.fbx",
	};

	for (const std::string& modelName : modelNames)
	{
		StaticMeshComponent staticMeshComponent = AssetLoader::getInstance().loadModel(modelName);

		auto entity = createEntity(Utility::basename(modelName));
		entity->addComponent<StaticMeshComponent>(staticMeshComponent);

		entity->attach(m_rootEntity);
	}

	m_entities["ogre"]->attach(m_entities["sponza"]);
	m_entities["dragon"]->attach(m_entities["ogre"]);
	m_entities["dragon"]->getComponent<TransformComponent>().position = glm::vec3(4.0f, 4.0f, 4.0f);
	m_entities["dragon"]->getComponent<TransformComponent>().rotation = glm::vec3(20.0f, 6.0f, 8.0f);
}

void Scene::destroy()
{
	m_registry.clear();
	m_entities.clear();
}

void Scene::pre()
{
	m_registry.view<StaticMeshComponent>().each([](auto entity, StaticMeshComponent& staticMeshComponent) {
		staticMeshComponent.initBatchResource();
	});
}

void Scene::begin()
{
	m_beginTime = std::chrono::steady_clock::now();
}

void Scene::tick(float deltaTime)
{
	m_camera->tick(deltaTime);

	m_entities["ogre"]->getComponent<TransformComponent>().rotation = glm::vec3(0.0f, 0.0f, time() * 90.0f);
	m_rootEntity->tick();

	m_registry.view<TransformComponent, StaticMeshComponent>().each([this](auto entity, TransformComponent& transformComponent, StaticMeshComponent& staticMeshComponent) {
		staticMeshComponent.batchResource->vpco.m = transformComponent.worldMatrix;
		staticMeshComponent.batchResource->vpco.mvp = m_camera->getViewPerspectiveMatrix() * transformComponent.worldMatrix;
		staticMeshComponent.batchResource->fpco.cameraPosition = m_camera->getPosition();
		staticMeshComponent.batchResource->fpco.lightDirection = glm::vec3(-1.0f, 1.0f, -1.0f);
	});
}

void Scene::end()
{
	
}

void Scene::post()
{
	m_registry.view<StaticMeshComponent>().each([](auto entity, auto& staticMeshComponent) {
		staticMeshComponent.destroyBatchResource();
	});
}

std::shared_ptr<Entity> Scene::createEntity(const std::string& name)
{
	auto entity = std::make_shared<Entity>(this, m_registry.create());
	entity->addComponent<TagComponent>(name);
	entity->addComponent<TransformComponent>();

	m_entities[name] = entity;
	return entity;
}

void Scene::onViewportSize(uint32_t width, uint32_t height)
{
	if (width != 0 && height != 0)
	{
		m_camera->setAspect((float)width / height);
	}
}

float Scene::time()
{
	std::chrono::steady_clock::time_point nowTime = std::chrono::steady_clock::now();
	return std::chrono::duration<float, std::chrono::seconds::period>(nowTime - m_beginTime).count();
}

void Scene::removeEntity(const std::string& name)
{
	m_entities.erase(name);
}