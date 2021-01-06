#include "engine.h"
#include "rendering/graphics_backend.h"
#include "rendering/renderer.h"
#include "rendering/shader_manager.h"
#include "io/asset_loader.h"
#include "input/input_manager.h"
#include "config/config_manager.h"
#include "scene.h"

void Engine::init()
{
	// ��ʼ��ͼ�κ�˺���Ⱦ��
	m_backend = std::make_shared<GraphicsBackend>();
	m_backend->setOnFramebufferResized(std::bind(&Engine::onViewportResized, this, std::placeholders::_1, std::placeholders::_2));
	m_backend->init(1280, 720);

	// ��ʼ�����ù�����
	ConfigManager::getInstance().init();

	// ��ʼ����ɫ��������
	ShaderManager::getInstance().init();

	// ��ʼ����Ⱦ��Դ����
	ResourceFactory::getInstance().init(m_backend);

	// ��ʼ�����������
	InputManager::getInstance().init(m_backend->getWindow());

	// ��ʼ����Ⱦ��
	m_renderer = std::make_shared<Renderer>();
	m_renderer->init(m_backend);

	// ��ʼ������
	m_scene = std::make_shared<class Scene>();

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
	m_beginTime = std::chrono::steady_clock::now();

	while (!glfwWindowShouldClose(m_backend->getWindow()))
	{
		glfwPollEvents();

		m_scene->tick(m_deltaTime);
		m_renderer->render();

		evaluateTime();
	}

	vkDeviceWaitIdle(m_backend->getDevice());
}

void Engine::destroy()
{
	InputManager::getInstance().destroy();
	ResourceFactory::getInstance().destroy();
	ShaderManager::getInstance().destroy();
	ConfigManager::getInstance().destroy();

	m_renderer->destroy();
	m_backend->destroy();
}

void Engine::evaluateTime()
{
	// ���㵱ǰ֡������ʱ�䣬����֡��
	std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
	m_deltaTime = static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - m_beginTime).count() * 1e-9);

	char title[100];
	snprintf(title, sizeof(title), "Bamboo Engine | FPS: %d", static_cast<int>(1.0f / m_deltaTime));
	glfwSetWindowTitle(m_backend->getWindow(), title);

	m_beginTime = std::chrono::steady_clock::now();
}

void Engine::onViewportResized(uint32_t width, uint32_t height)
{
	m_scene->onViewportSize()
}