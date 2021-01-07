#include "engine.h"
#include "rendering/graphics_backend.h"
#include "rendering/renderer.h"
#include "rendering/shader_manager.h"
#include "io/asset_loader.h"
#include "input/input_manager.h"
#include "config/config_manager.h"
#include "utility/utility.h"
#include "scene.h"

void Engine::init()
{
	// 初始化配置管理器
	ConfigManager::getInstance().init();
	uint32_t width, height;
	ConfigManager::getInstance().getResolution(width, height);

	// 初始化着色器管理器
	ShaderManager::getInstance().init();

	// 初始化图形后端和渲染器
	m_backend = std::make_shared<GraphicsBackend>();
	m_backend->setOnFramebufferResized(std::bind(&Engine::onViewportResized, this, std::placeholders::_1, std::placeholders::_2));
	m_backend->init(width, height);

	// 创建渲染器
	m_renderer = std::make_shared<Renderer>();

	// 初始化渲染资源工厂
	ResourceFactory::getInstance().init(m_backend, m_renderer);

	// 初始化渲染器
	m_renderer->init(m_backend);

	// 初始化输入管理器
	InputManager::getInstance().init(m_backend->getWindow());

	// 初始化场景
	m_scene = std::make_shared<class Scene>();
	m_scene->init(width, height);

	// 加载模型资源，生成组件
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
		
		Entity entity = m_scene->createEntity(Utility::basename(modelName));
		entity.addComponent<StaticMeshComponent>(staticMeshComponent);
	}
}

void Engine::run()
{
	m_beginTime = std::chrono::steady_clock::now();

	m_scene->pre();
	m_scene->begin();

	while (!glfwWindowShouldClose(m_backend->getWindow()))
	{
		glfwPollEvents();

		m_scene->tick(m_deltaTime);
		m_renderer->render();

		evaluateTime();
	}

	vkDeviceWaitIdle(m_backend->getDevice());

	m_scene->end();
	m_scene->post();
}

void Engine::destroy()
{
	ResourceFactory::getInstance().destroy();
	InputManager::getInstance().destroy();
	ShaderManager::getInstance().destroy();
	ConfigManager::getInstance().destroy();

	m_scene->destroy();
	m_renderer->destroy();
	m_backend->destroy();
}

void Engine::evaluateTime()
{
	// 计算当前帧所花的时间，计算帧率
	std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
	m_deltaTime = static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - m_beginTime).count() * 1e-9);

	char title[100];
	snprintf(title, sizeof(title), "Bamboo Engine | FPS: %d", static_cast<int>(1.0f / m_deltaTime));
	glfwSetWindowTitle(m_backend->getWindow(), title);

	m_beginTime = std::chrono::steady_clock::now();
}

void Engine::onViewportResized(uint32_t width, uint32_t height)
{
	m_renderer->onFramebufferResized();
	m_scene->onViewportSize(width, height);
}