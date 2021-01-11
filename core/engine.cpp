#include "engine.h"
#include "rendering/graphics_backend.h"
#include "rendering/renderer.h"
#include "rendering/shader_manager.h"
#include "input/input_manager.h"
#include "config/config_manager.h"
#include "scene.h"
#include <thread>

void Engine::init()
{
	// ��ʼ�����ù�����
	ConfigManager::getInstance().init();
	uint32_t width, height;
	ConfigManager::getInstance().getResolution(width, height);

	// ��ʼ����ɫ��������
	ShaderManager::getInstance().init();

	// ��ʼ��ͼ�κ�˺���Ⱦ��
	m_backend = std::make_shared<GraphicsBackend>();
	m_backend->setOnFramebufferResized(std::bind(&Engine::onViewportResized, this, std::placeholders::_1, std::placeholders::_2));
	m_backend->init(width, height);

	// ��ʼ����Ⱦ��Դ����
	ResourceFactory::getInstance().init(m_backend);

	// ��ʼ����Ⱦ��
	m_renderer = std::make_shared<Renderer>();
	m_renderer->init(m_backend);

	// ��ʼ�����������
	InputManager::getInstance().init(m_backend->getWindow());

	// ��ʼ������
	m_scene = std::make_shared<class Scene>();
	m_scene->init(m_renderer);
}

void Engine::run()
{
	m_scene->pre();
	m_scene->begin();

	m_lastTime = std::chrono::steady_clock::now();
	m_lastTimeAfterSleep = std::chrono::steady_clock::now();
	while (!glfwWindowShouldClose(m_backend->getWindow()))
	{
		glfwPollEvents();
		evaluateTime();

		m_renderer->wait();
		m_scene->tick(m_deltaTime);
		m_renderer->update();
		m_renderer->submit();
		m_renderer->present();
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

void Engine::evaluateTime(int targetFPS)
{
	// ���㵱ǰ֡������ʱ�䣬����֡��
	std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
	m_deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - m_lastTime).count();
	m_lastTime = currentTime;

	if (targetFPS != 0)
	{
		long long renderTime = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - m_lastTimeAfterSleep).count();
		long long sleepTime = std::max(static_cast<long long>(1e9 / targetFPS) - renderTime, 0ll);
		m_lastTimeAfterSleep = currentTime + std::chrono::nanoseconds(sleepTime);

		if (sleepTime > 0ll)
		{
			std::this_thread::sleep_for(std::chrono::nanoseconds(sleepTime));
		}
		//m_lastTimeAfterSleep = std::chrono::steady_clock::now();
	}

	char title[100];
	snprintf(title, sizeof(title), "Bamboo Engine | FPS: %d", static_cast<int>(1.0f / m_deltaTime));
	glfwSetWindowTitle(m_backend->getWindow(), title);
}

void Engine::onViewportResized(uint32_t width, uint32_t height)
{
	m_renderer->onFramebufferResized();
}