#pragma once

#include "rendering/graphics_backend.h"
#include "rendering/renderer.h"

class Engine
{
public:
	void init();
	void run();
	void destroy();

private:
	GraphicsBackend m_backend;
	Renderer m_renderer;
	class Camera* m_camera;

	float m_deltaTime;
};

