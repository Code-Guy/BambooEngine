#pragma once

#include <map>

#include "swapchain.h"
#include "render_pass.h"
#include "framebuffer.h"
#include "static_mesh_pipeline.h"
#include "skeletal_mesh_pipeline.h"

class Renderer
{
public:
	void init(std::shared_ptr<class GraphicsBackend>& backend);
	void destroy();

	void wait();
	void update();
	void submit();
	void present();

	std::shared_ptr<class GraphicsBackend> getBackend() { return m_backend; }
	std::shared_ptr<Pipeline> getPipeline(EPipelineType pipelineType) { return m_pipelines[pipelineType]; }
	uint32_t getImageIndex() { return m_imageIndex; }
	glm::ivec2 getViewportSize();

	void onFramebufferResized() { m_framebufferResized = true; }

private:
	void createCommandPool();
	void createMsaaResources();
	void createDepthResources();
	void createFramebuffers();
	void createCommandBuffers();
	void createSyncObjects();

	void recreateSwapchain();
	void cleanupSwapchain();

	std::shared_ptr<GraphicsBackend> m_backend;

	VkFormat m_depthFormat;
	VmaImageView m_depthImageView;
	VmaImageView m_msaaImageView;

	VkCommandPool m_commandPool;
	std::vector<VkCommandBuffer> m_commandBuffers;

	Swapchain m_swapchain; // 交换链对象
	RenderPass m_renderPass; // 渲染批次对象
	std::vector<Framebufer> m_swapchainFramebuffers; // 帧缓存对象列表
	std::map<EPipelineType, std::shared_ptr<Pipeline>> m_pipelines; // 渲染流水线对象字典

	const size_t MAX_FRAMES_IN_FLIGHT = 2;
	std::vector<VkSemaphore> m_imageAvailableSemaphores;
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkSemaphore> m_signalSemaphores;
	std::vector<VkFence> m_inFlightFences;
	std::vector<VkFence> m_imagesInFlight;
	size_t m_currentFrame = 0;
	uint32_t m_imageIndex;

	bool m_framebufferResized;
};