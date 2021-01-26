#include "engine.h"
#include "rendering/graphics_backend.h"
#include "rendering/renderer.h"
#include "rendering/shader_manager.h"
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

	// 初始化渲染资源工厂
	ResourceFactory::getInstance().init(m_backend);

	// 初始化渲染器
	m_renderer = std::make_shared<Renderer>();
	m_renderer->init(m_backend);

	// 初始化输入管理器
	InputManager::getInstance().init(m_backend->getWindow());

	// 初始化场景
	m_scene = std::make_shared<class Scene>();
	m_scene->init(m_renderer);
}

void Engine::run()
{
	m_scene->pre();
	m_scene->begin();

	m_lastTime = std::chrono::high_resolution_clock::now();
	while (true)
	{
		auto beginTime = std::chrono::high_resolution_clock::now();
		if (glfwWindowShouldClose(m_backend->getWindow()))
		{
			break;
		}
		glfwPollEvents();

		m_renderer->wait();
		m_scene->tick(m_deltaTime);
		m_renderer->update();
		m_renderer->submit();
		m_renderer->present();

		updateTitle();
		auto endTime = std::chrono::high_resolution_clock::now();
		m_renderTime = std::chrono::duration<float, std::chrono::seconds::period>(endTime - beginTime).count();

		evaluateTime(true);
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

void Engine::updateTitle()
{
	char title[100];
	snprintf(title, sizeof(title), "Bamboo Engine | FPS: %d", static_cast<int>(1.0f / m_deltaTime));
	glfwSetWindowTitle(m_backend->getWindow(), title);
}

void Engine::evaluateTime(bool limitFrameRate)
{
	// Governing the Frame Rate
	if (limitFrameRate)
	{
		float sleepTime = std::max(1.0f / 60.0f - m_renderTime, 0.0f);
		if (sleepTime > 0.0f)
		{
			Utility::preciseSleep(sleepTime);
		}
	}

	// 计算当前帧所花的时间，计算帧率
	auto currentTime = std::chrono::high_resolution_clock::now();
	m_deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - m_lastTime).count();
	m_lastTime = currentTime;
}

void Engine::onViewportResized(uint32_t width, uint32_t height)
{
	m_renderer->onFramebufferResized();
}