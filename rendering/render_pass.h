#pragma once

#include <vulkan/vulkan.h>

#include "graphics_backend.h"

class RenderPass
{
public:
	void init(std::shared_ptr<GraphicsBackend>& backend, VkFormat colorFormat, VkFormat depthFormat);
	void destroy();

	VkRenderPass get() { return m_renderPass; }

private:
	std::shared_ptr<GraphicsBackend> m_backend;
	VkRenderPass m_renderPass;
};