#include "engine.h"
#include "rendering/graphics_backend.h"
#include "rendering/renderer.h"
#include "rendering/shader_manager.h"

#include "io/asset_loader.h"
#include "input/input_manager.h"
#include "config/config_manager.h"

#include "entity/camera.h"

void Engine::init()
{
	// ��ʼ��ͼ�κ�˺���Ⱦ��
	m_backend = new GraphicsBackend;
	m_backend->init(1280, 720);

	// ��ʼ�����ù�����
	ConfigManager::getInstance().init();

	// ��ʼ����ɫ��������
	ShaderManager::getInstance().init();

	// ��ʼ����Ⱦ��Դ����
	ResourceFactory::getInstance().init(m_backend);

	// ��ʼ�����������
	InputManager::getInstance().init(m_backend->getWindow());

	// ��ʼ�������
	m_camera = new Camera(glm::vec3(12.0f, 8.0f, 5.0f), 220.0f, -18.0f, 50.0f, 0.1f);
	m_camera->setFovy(45.0f);
	m_camera->setClipping(0.1f, 1000.0f);

	InputManager::getInstance().registerKeyPressed(std::bind(&Camera::onKeyPressed, m_camera, std::placeholders::_1));
	InputManager::getInstance().registerKeyReleased(std::bind(&Camera::onKeyReleased, m_camera, std::placeholders::_1));
	InputManager::getInstance().registerMouseOffseted(std::bind(&Camera::onMouseOffseted, m_camera, std::placeholders::_1, std::placeholders::_2));
	InputManager::getInstance().registerMousePressed(std::bind(&Camera::onMousePressed, m_camera, std::placeholders::_1));
	InputManager::getInstance().registerMouseReleased(std::bind(&Camera::onMouseReleased, m_camera, std::placeholders::_1));

	// ��ʼ����Ⱦ��
	m_renderer = new Renderer;
	m_renderer->init(m_backend, m_camera);

	std::vector<std::string> modelNames = {
		//"asset/model/ground/ground.fbx",
		//"asset/model/dinosaur/dinosaur.fbx",
		//"asset/model/armadillo/armadillo.fbx",
		//"asset/model/dragon/dragon.fbx",
		//"asset/model/ogre/ogre.fbx",
		"asset/model/sponza/sponza.fbx",
	};

	std::vector<BatchResource*> batchResources;
	for (const std::string& modelName : modelNames)
	{
		// ����ģ����Դ���������
		std::shared_ptr<StaticMeshComponent> staticMeshComponent = AssetLoader::getInstance().loadModel(modelName);

		// ͨ�����������Ⱦ��Դ
		batchResources.push_back(ResourceFactory::getInstance().createBatchResource(staticMeshComponent));
	}

	// ������Ⱦ����Ⱦ��Դ
	m_renderer->setBatchResources(batchResources);
}

void Engine::run()
{
	std::chrono::steady_clock::time_point beginTime = std::chrono::steady_clock::now();

	while (!glfwWindowShouldClose(m_backend->getWindow()))
	{
		glfwPollEvents();

		m_camera->tick(m_deltaTime);
		m_renderer->render();

		// ���㵱ǰ֡������ʱ�䣬����֡��
		std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
		m_deltaTime = static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - beginTime).count() * 1e-9);

		char title[100];
		snprintf(title, sizeof(title), "Bamboo Engine | FPS: %d", static_cast<int>(1.0f / m_deltaTime));
		glfwSetWindowTitle(m_backend->getWindow(), title);

		beginTime = std::chrono::steady_clock::now();
	}

	vkDeviceWaitIdle(m_backend->getDevice());
}

void Engine::destroy()
{
	m_renderer->destroy();
	InputManager::getInstance().destroy();
	ResourceFactory::getInstance().destroy();
	ShaderManager::getInstance().destroy();
	ConfigManager::getInstance().destroy();
	m_backend->destroy();

	delete m_camera;
	delete m_renderer;
	delete m_backend;
}