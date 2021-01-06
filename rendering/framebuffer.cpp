#include "framebuffer.h"

void Framebufer::init(std::shared_ptr<GraphicsBackend>& backend, VkRenderPass renderPass, VkExtent2D extent, const std::vector<VkImageView>& attachments)
{
	m_backend = backend;

	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = extent.width;
	framebufferInfo.height = extent.height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(m_backend->getDevice(), &framebufferInfo, nullptr, &m_framebuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create framebuffer!");
	}
}

void Framebufer::destroy()
{
	vkDestroyFramebuffer(m_backend->getDevice(), m_framebuffer, nullptr);
}
