#pragma once

#include <vulkan/vulkan.h>

#include "graphics_backend.h"

class RenderPass
{
public:
	void init(GraphicsBackend* backend, VkFormat colorFormat, VkFormat depthFormat);
	void destroy();

	VkRenderPass get() { return m_renderPass; }

private:
	GraphicsBackend* m_backend;
	VkRenderPass m_renderPass;
};