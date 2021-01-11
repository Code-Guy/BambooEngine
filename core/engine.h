#pragma once

#include <chrono>
#include <memory>

class Engine
{
public:
	void init();
	void run();
	void destroy();

private:
	void evaluateTime(int targetFPS = 0);
	void onViewportResized(uint32_t width, uint32_t height);

	std::shared_ptr<class GraphicsBackend> m_backend;
	std::shared_ptr<class Renderer> m_renderer;
	std::shared_ptr<class Scene> m_scene;

	std::chrono::steady_clock::time_point m_lastTime;
	float m_deltaTime;
};