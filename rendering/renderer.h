#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#include "swapchain.h"
#include "render_pass.h"
#include "framebuffer.h"
#include "static_mesh_pipeline.h"
#include "batch_resource.h"

class Renderer
{
public:
	void init(class GraphicsBackend* graphicBackend, class Camera* camera);
	void render();
	void destroy();

	void setBatchResources(std::vector<BatchResource*> batchResources);

private:
	void createCommandPool();
	void createMsaaResources();
	void createDepthResources();
	void createFramebuffers();
	void createCommandBuffers();
	void createSyncObjects();

	void recreateSwapchain();
	void cleanupSwapchain();

	void cleanupBatchResource();

	void updateCommandBuffer(uint32_t imageIndex);
	void updatePushConstants(VkCommandBuffer commandBuffer, size_t batchIndex);

	class GraphicsBackend* m_backend;
	class Camera* m_camera;

	VkFormat m_depthFormat;
	
	VmaImageView m_depthImageView;
	VmaImageView m_msaaImageView;

	VkCommandPool m_commandPool;
	std::vector<VkCommandBuffer> m_commandBuffers;

	Swapchain m_swapchain; // 交换链对象
	RenderPass m_renderPass; // 渲染批次对象
	std::vector<Framebufer> m_swapchainFramebuffers; // 帧缓存对象列表
	StaticMeshPipeline m_staticMeshPipeline; // 渲染流水线对象

	const size_t MAX_FRAMES_IN_FLIGHT = 2;
	std::vector<VkSemaphore> m_imageAvailableSemaphores;
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkFence> m_inFlightFences;
	std::vector<VkFence> m_imagesInFlight;
	size_t m_currentFrame = 0;

	std::vector<BatchResource*> m_batchResources;
};