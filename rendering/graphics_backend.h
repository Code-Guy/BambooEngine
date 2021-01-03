#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan_memory_allocator/vk_mem_alloc.h>

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <optional>

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() const
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;

	bool isComplete() const
	{
		return !formats.empty() && !presentModes.empty();
	}
};

class GraphicsBackend
{
public:
	void init(uint32_t width, uint32_t height);
	void destroy();

	VkInstance getInstance() { return m_instance; }
	VkDevice getDevice() { return m_device; }
	VkPhysicalDevice getPhysicalDevice() { return m_physicalDevice; }
	VkSurfaceKHR getSurface() { return m_surface; }
	GLFWwindow* getWindow() { return m_window; }
	VmaAllocator getAllocator() { return m_vmaAllocator; }
	VkQueue getGraphicsQueue() { return m_graphicsQueue; }
	VkQueue getPresentQueue() { return m_presentQueue; }

	const VkPhysicalDeviceProperties& getPhysicalDeviceProperties() { return m_physicalDeviceProperties; }
	SwapChainSupportDetails getSwapChainSupport();
	const QueueFamilyIndices& getQueueFamilyIndices() { return m_queueFamilyIndices; }
	VkFormat getSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	VkSampleCountFlagBits getMsaaSamples() { return m_msaaSamples; }

	bool getFramebufferResized() { return m_framebufferResized; }
	void setFramebufferResized(bool bFlag) { m_framebufferResized = bFlag; }

private:
	void initWindow();
	void createInstance();
	void setupDebugMessenger();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createVmaAllocator();
	
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredInstanceExtensions();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	bool checkPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice);
	bool checkPhysicalDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
	QueueFamilyIndices queryQueueFamilies(VkPhysicalDevice physicalDevice);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice);
	VkSampleCountFlagBits queryMaxSampleCount();

	static void onFramebufferResize(GLFWwindow* window, int width, int height);
	static VKAPI_ATTR VkBool32 VKAPI_CALL onDebugMessageOutput(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	VkDevice m_device;
	VkPhysicalDevice m_physicalDevice;
	VkSurfaceKHR m_surface;
	GLFWwindow* m_window;
	VmaAllocator m_vmaAllocator;
	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;
	
	VkPhysicalDeviceProperties m_physicalDeviceProperties;
	QueueFamilyIndices m_queueFamilyIndices;

	VkSampleCountFlagBits m_msaaSamples;

	uint32_t m_width;
	uint32_t m_height;
	bool m_framebufferResized;

	// 调试层列表
	const std::vector<const char*> m_validationLayers =
	{
		"VK_LAYER_KHRONOS_validation"
	};

	// 设备扩展列表
	const std::vector<const char*> m_deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
		VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME
	};

#ifdef NDEBUG
	const bool m_enableValidationLayers = false;
#else
	const bool m_enableValidationLayers = true;
#endif
};