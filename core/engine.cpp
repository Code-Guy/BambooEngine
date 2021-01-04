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
	// 初始化图形后端和渲染器
	m_backend = new GraphicsBackend;
	m_backend->init(1280, 720);

	// 初始化配置管理器
	ConfigManager::getInstance().init();

	// 初始化着色器管理器
	ShaderManager::getInstance().init();

	// 初始化渲染资源工厂
	ResourceFactory::getInstance().init(m_backend);

	// 初始化输入管理器
	InputManager::getInstance().init(m_backend->getWindow());

	// 初始化摄像机
	m_camera = new Camera(glm::vec3(12.0f, 8.0f, 5.0f), 220.0f, -18.0f, 50.0f, 0.1f);
	m_camera->setFovy(45.0f);
	m_camera->setClipping(0.1f, 1000.0f);

	InputManager::getInstance().registerKeyPressed(std::bind(&Camera::onKeyPressed, m_camera, std::placeholders::_1));
	InputManager::getInstance().registerKeyReleased(std::bind(&Camera::onKeyReleased, m_camera, std::placeholders::_1));
	InputManager::getInstance().registerMouseOffseted(std::bind(&Camera::onMouseOffseted, m_camera, std::placeholders::_1, std::placeholders::_2));
	InputManager::getInstance().registerMousePressed(std::bind(&Camera::onMousePressed, m_camera, std::placeholders::_1));
	InputManager::getInstance().registerMouseReleased(std::bind(&Camera::onMouseReleased, m_camera, std::placeholders::_1));

	// 初始化渲染器
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
		// 加载模型资源，生成组件
		std::shared_ptr<StaticMeshComponent> staticMeshComponent = AssetLoader::getInstance().loadModel(modelName);

		// 通过组件生成渲染资源
		batchResources.push_back(ResourceFactory::getInstance().createBatchResource(staticMeshComponent));
	}

	// 设置渲染器渲染资源
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

		// 计算当前帧所花的时间，计算帧率
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