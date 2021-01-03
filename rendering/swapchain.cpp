#include "swapchain.h"
#include "resource_factory.h"
#include <algorithm>
#include <boost/format.hpp>

void Swapchain::init(GraphicsBackend* backend)
{
	m_backend = backend;

	SwapChainSupportDetails details = m_backend->getSwapChainSupport();

	VkSurfaceFormatKHR surfaceFormat = pickSwapSurfaceFormat(details.formats);
	VkPresentModeKHR presentMode = pickSwapPresentMode(details.presentModes);
	VkExtent2D extent = pickSwapExtent(details.capabilities);

	m_swapchainImageFormat = surfaceFormat.format;
	m_swapchainExtent = extent;

	// +1 to avoid wating for another image to render to
	uint32_t imageCount = details.capabilities.minImageCount + 1;
	if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount)
	{
		imageCount = details.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_backend->getSurface();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	const QueueFamilyIndices& queueFamilyIndices = m_backend->getQueueFamilyIndices();
	uint32_t pQueueFamilyIndices[] = { queueFamilyIndices.graphicsFamily.value(), queueFamilyIndices.presentFamily.value() };

	if (queueFamilyIndices.graphicsFamily != queueFamilyIndices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = pQueueFamilyIndices;
	}
	else
	{
		// 大部分硬件graphics和present公用一个queue，提高效率
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = details.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_backend->getDevice(), &createInfo, nullptr, &m_swapchain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(m_backend->getDevice(), m_swapchain, &imageCount, nullptr);
	if (m_swapchainImages.size() == 0)
	{
		m_swapchainImages.resize(imageCount);
	}
	else if(m_swapchainImages.size() != imageCount)
	{
		throw std::runtime_error((boost::format("the image count of new swap chain(%d) is different with the old one(%d)!") % imageCount % m_swapchainImages.size()).str());
	}

	vkGetSwapchainImagesKHR(m_backend->getDevice(), m_swapchain, &imageCount, m_swapchainImages.data());

	// 创建交换链ImageView
	m_swapchainImageViews.resize(m_swapchainImages.size());
	for (size_t i = 0; i < m_swapchainImages.size(); ++i)
	{
		m_swapchainImageViews[i] = ResourceFactory::getInstance().createImageView(m_swapchainImages[i], m_swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

void Swapchain::destroy()
{
	for (auto& swapchainImageView : m_swapchainImageViews)
	{
		vkDestroyImageView(m_backend->getDevice(), swapchainImageView, nullptr);
	}

	vkDestroySwapchainKHR(m_backend->getDevice(), m_swapchain, nullptr);
}

VkSurfaceFormatKHR Swapchain::pickSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats)
{
	for (const auto& availableSurfaceFormat : availableSurfaceFormats)
	{
		if (availableSurfaceFormat.format == m_preferredSurfaceFormat && availableSurfaceFormat.colorSpace == m_preferredColorSpace)
		{
			return availableSurfaceFormat;
		}
	}

	return availableSurfaceFormats[0];
}

VkPresentModeKHR Swapchain::pickSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == m_mostRreferredPresentMode)
		{
			return availablePresentMode;
		}
	}
	return m_lessRreferredPresentMode;
}

VkExtent2D Swapchain::pickSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}

	int width, height;
	glfwGetFramebufferSize(m_backend->getWindow(), &width, &height);

	VkExtent2D actualExtent =
	{
		static_cast<uint32_t>(width),
		static_cast<uint32_t>(height),
	};

	actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

	return actualExtent;
}
