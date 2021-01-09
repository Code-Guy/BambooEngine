#pragma once

#include <vulkan/vulkan.h>
#include "graphics_backend.h"

class Framebufer
{
public:
	void init(std::shared_ptr<class GraphicsBackend>& backend, VkRenderPass renderPass, VkExtent2D extent, const std::vector<VkImageView>& attachments);
	void destroy();

	VkFramebuffer get() { return m_framebuffer; }

private:
	std::shared_ptr<class GraphicsBackend> m_backend;

	VkFramebuffer m_framebuffer;
};