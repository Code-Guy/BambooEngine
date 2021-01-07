#include "scene.h"
#include "input/input_manager.h"
#include "component/component.h"
#include "io/asset_loader.h"
#include "utility/utility.h"

void Scene::init(uint32_t width, uint32_t height)
{
	m_registry = std::make_shared<entt::registry>();

	// ��ʼ�������
	m_camera = std::make_unique<Camera>(glm::vec3(12.0f, 8.0f, 5.0f), 220.0f, -18.0f, 50.0f, 0.1f);
	m_camera->setFovy(45.0f);
	m_camera->setClipping(0.1f, 1000.0f);
	m_camera->setAspect((float)width / height);

	// ������������¼�
	InputManager::getInstance().registerKeyPressed(std::bind(&Camera::onKeyPressed, m_camera.get(), std::placeholders::_1));
	InputManager::getInstance().registerKeyReleased(std::bind(&Camera::onKeyReleased, m_camera.get(), std::placeholders::_1));
	InputManager::getInstance().registerMouseOffseted(std::bind(&Camera::onMouseOffseted, m_camera.get(), std::placeholders::_1, std::placeholders::_2));
	InputManager::getInstance().registerMousePressed(std::bind(&Camera::onMousePressed, m_camera.get(), std::placeholders::_1));
	InputManager::getInstance().registerMouseReleased(std::bind(&Camera::onMouseReleased, m_camera.get(), std::placeholders::_1));

	// ��Ӹ��ڵ�
	m_rootEntity = createEntity("root");

	// ����ģ����Դ���������
	std::vector<std::string> modelNames = {
		//"asset/model/ground/ground.fbx",
		//"asset/model/dinosaur/dinosaur.fbx",
		//"asset/model/armadillo/armadillo.fbx",
		//"asset/model/dragon/dragon.fbx",
		//"asset/model/ogre/ogre.fbx",
		"asset/model/sponza/sponza.fbx",
	};

	for (const std::string& modelName : modelNames)
	{
		StaticMeshComponent staticMeshComponent = AssetLoader::getInstance().loadModel(modelName);

		Entity entity = createEntity(Utility::basename(modelName));
		entity.addComponent<StaticMeshComponent>(staticMeshComponent);

		entity.attach(&m_rootEntity);
	}
}

void Scene::destroy()
{

}

void Scene::pre()
{
	m_registry->view<StaticMeshComponent>().each([](auto entity, StaticMeshComponent& staticMeshComponent) {
		staticMeshComponent.initBatchResource();
	});
}

void Scene::begin()
{

}

void Scene::tick(float deltaTime)
{
	m_camera->tick(deltaTime);
	m_rootEntity.tick();

	m_registry->view<TransformComponent, StaticMeshComponent>().each([this](auto entity, TransformComponent& transformComponent, StaticMeshComponent& staticMeshComponent) {
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
	m_registry->view<StaticMeshComponent>().each([](auto entity, auto& staticMeshComponent) {
		staticMeshComponent.destroyBatchResource();
	});
}

Entity Scene::createEntity(const std::string& name)
{
	Entity entity = Entity(m_registry, m_registry->create());
	entity.addComponent<TagComponent>(name);
	entity.addComponent<TransformComponent>();

	return entity;
}

void Scene::onViewportSize(uint32_t width, uint32_t height)
{
	m_camera->setAspect((float)width / height);
}