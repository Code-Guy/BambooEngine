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
	GraphicsBackend backend;
	Renderer renderer;
};

