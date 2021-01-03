#pragma once

#include <vulkan/vulkan.h>
#include "graphics_backend.h"

class Swapchain
{
public:
	void init(GraphicsBackend* backend);
	void destroy();

	VkSwapchainKHR get() { return m_swapchain; }

	VkImageView getImageView(size_t index) { return m_swapchainImageViews[index]; }
	VkFormat getFormat() { return m_swapchainImageFormat; }
	VkExtent2D getExtent() { return m_swapchainExtent; }
	size_t getSize() { return m_swapchainImages.size(); }

private:
	VkSurfaceFormatKHR pickSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR pickSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D pickSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	class GraphicsBackend* m_backend;

	VkSwapchainKHR m_swapchain;
	std::vector<VkImage> m_swapchainImages;
	std::vector<VkImageView> m_swapchainImageViews;
	VkFormat m_swapchainImageFormat;
	VkExtent2D m_swapchainExtent;

	const VkFormat m_preferredSurfaceFormat = VK_FORMAT_B8G8R8A8_SRGB;
	const VkColorSpaceKHR m_preferredColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	const VkPresentModeKHR m_mostRreferredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	const VkPresentModeKHR m_lessRreferredPresentMode = VK_PRESENT_MODE_FIFO_KHR;
};