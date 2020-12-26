#pragma once

class Engine
{
public:
	void init();
	void run();
	void destroy();

private:
	class GraphicsBackend* m_backend;
	class Renderer* m_renderer;
	class Camera* m_camera;

	float m_deltaTime;
};

