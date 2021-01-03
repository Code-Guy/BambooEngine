#pragma once

#include <vulkan/vulkan.h>
#include "graphics_backend.h"

class Framebufer
{
public:
	void init(GraphicsBackend* backend, VkRenderPass renderPass, VkExtent2D extent, const std::vector<VkImageView>& attachments);
	void destroy();

	VkFramebuffer get() { return m_framebuffer; }

private:
	class GraphicsBackend* m_backend;

	VkFramebuffer m_framebuffer;
};