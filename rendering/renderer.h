#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <algorithm>

#include "graphics_backend.h"
#include "batch_resource.h"
#include "resource_factory.h"

class Renderer
{
public:
	void init(class GraphicsBackend* graphicBackend, class Camera* camera);
	void render();
	void destroy();

	void setBatchResources(std::vector<BatchResource> batchResources);

private:
	void createSwapChain();
	void createImageViews();
	void createRenderPass();
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	void createCommandPool();
	void createDescriptorPool();
	void createMsaaResources();
	void createDepthResources();
	void createFramebuffers();
	void createCommandBuffers();
	void createSyncObjects();

	void recreateSwapchain();
	void createSwapchainBatchResource();
	void cleanupSwapchain();
	void cleanupSwapchainBatchResource();
	void cleanupBatchResource();

	void updateUniformBuffer(uint32_t imageIndex);

	VkSurfaceFormatKHR pickSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR pickSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D pickSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
	VkFormat querySupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	class GraphicsBackend* m_backend;

	const VkFormat m_preferredSurfaceFormat = VK_FORMAT_B8G8R8A8_SRGB;
	const VkColorSpaceKHR m_preferredColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	const VkPresentModeKHR m_mostRreferredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	const VkPresentModeKHR m_lessRreferredPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	VkFormat m_depthFormat;

	VkSwapchainKHR m_swapchain;
	std::vector<VkImage> m_swapchainImages;
	std::vector<VkImageView> m_swapchainImageViews;
	std::vector<VkFramebuffer> m_swapchainFramebuffers;
	VkFormat m_swapchainImageFormat;
	VkExtent2D m_swapchainExtent;
	VkRenderPass m_renderPass;

	VkDescriptorSetLayout m_descriptorSetLayout;
	VkDescriptorPool m_descriptorPool;
	VkPipelineLayout m_pipelineLayout;
	VkPipeline m_graphicsPipeline;

	VmaImageView m_depthImageView;
	VmaImageView m_msaaImageView;

	VkCommandPool m_commandPool;
	std::vector<VkCommandBuffer> m_commandBuffers;

	const size_t MAX_FRAMES_IN_FLIGHT = 2;
	std::vector<VkSemaphore> m_imageAvailableSemaphores;
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkFence> m_inFlightFences;
	std::vector<VkFence> m_imagesInFlight;
	size_t m_currentFrame = 0;

	std::vector<BatchResource> m_batchResources;
	class Camera* m_camera;
};