#include "engine.h"
#include "io/asset_loader.h"
#include "input/input_manager.h"
#include "entity/camera.h"

void Engine::init()
{
	// ��ʼ��ͼ�κ�˺���Ⱦ��
	m_backend.init(1280, 720);

	// ��ʼ����Ⱦ��Դ����
	ResourceFactory::getInstance().init(&m_backend);

	// ��ʼ�����������
	InputManager::getInstance().init(m_backend.getWindow());

	// ��ʼ�������
	m_camera = new Camera(glm::vec3(12.0f, 8.0f, 5.0f), 220.0f, -18.0f, 10.0f, 0.1f);
	m_camera->setFovy(45.0f);
	m_camera->setClipping(0.1f, 100.0f);

	InputManager::getInstance().registerKeyPressed(std::bind(&Camera::onKeyPressed, m_camera, std::placeholders::_1));
	InputManager::getInstance().registerKeyReleased(std::bind(&Camera::onKeyReleased, m_camera, std::placeholders::_1));
	InputManager::getInstance().registerMouseOffseted(std::bind(&Camera::onMouseOffseted, m_camera, std::placeholders::_1, std::placeholders::_2));
	InputManager::getInstance().registerMousePressed(std::bind(&Camera::onMousePressed, m_camera, std::placeholders::_1));
	InputManager::getInstance().registerMouseReleased(std::bind(&Camera::onMouseReleased, m_camera, std::placeholders::_1));

	// ��ʼ����Ⱦ��
	m_renderer.init(&m_backend, m_camera);

	// ����ģ����Դ���������
	std::vector<StaticMeshComponent> staticMeshComponents;
	std::vector<std::string> modelNames = {
		"asset/model/ground/ground.fbx",
		"asset/model/dinosaur/dinosaur.fbx",
		"asset/model/armadillo/armadillo.fbx",
		"asset/model/dragon/dragon.fbx",
		"asset/model/ogre/ogre.fbx"
	};

	for (const std::string& modelName : modelNames)
	{
		std::vector<StaticMeshComponent> components;
		AssetLoader::getInstance().loadModel(modelName, components);
		staticMeshComponents.insert(staticMeshComponents.end(), components.begin(), components.end());
	}
	
	// ͨ�����������Ⱦ��Դ
	std::vector<BatchResource> batchResources(staticMeshComponents.size());
	for (size_t i = 0; i < staticMeshComponents.size(); ++i)
	{
		ResourceFactory::getInstance().createBatchResource(staticMeshComponents[i], batchResources[i]);
	}

	// ������Ⱦ����Ⱦ��Դ
	m_renderer.setBatchResources(batchResources);
}

void Engine::run()
{
	std::chrono::steady_clock::time_point beginTime = std::chrono::steady_clock::now();

	while (!glfwWindowShouldClose(m_backend.getWindow()))
	{
		glfwPollEvents();

		m_camera->tick(m_deltaTime);
		m_renderer.render();

		// ���㵱ǰ֡������ʱ�䣬����֡��
		std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
		m_deltaTime = static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - beginTime).count() * 1e-9);

		char title[100];
		snprintf(title, sizeof(title), "Bamboo Engine | FPS: %d", static_cast<int>(1.0f / m_deltaTime));
		glfwSetWindowTitle(m_backend.getWindow(), title);

		beginTime = std::chrono::steady_clock::now();
	}

	vkDeviceWaitIdle(m_backend.getDevice());
}

void Engine::destroy()
{
	delete m_camera;
	m_renderer.destroy();
	ResourceFactory::getInstance().destroy();
	m_backend.destroy();
}
