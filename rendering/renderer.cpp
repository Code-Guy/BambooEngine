#include "renderer.h"
#include "entity/camera.h"

#define MAX_DESC_NUM 100

void Renderer::init(GraphicsBackend* graphicBackend, Camera* camera)
{
	m_backend = graphicBackend;
	m_camera = camera;

	m_swapchain.init(m_backend);
	m_camera->setAspect(m_swapchain.getExtent().width / static_cast<float>(m_swapchain.getExtent().height));

	m_depthFormat = m_backend->getSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	m_renderPass.init(graphicBackend, m_swapchain.getFormat(), m_depthFormat);
	m_staticMeshPipeline.init(graphicBackend, m_renderPass.get());

	createCommandPool();
	createMsaaResources();
	createDepthResources();
	createFramebuffers();
	createCommandBuffers();
	createSyncObjects();
}

void Renderer::render()
{
	// 等待指令提交完毕
	vkWaitForFences(m_backend->getDevice(), 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

	// 获取当前可用Image的索引，这里会等待交换链有空闲的Image
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(m_backend->getDevice(), m_swapchain.get(), UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);

	// 检测是否要重建交换链
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapchain();
		return;
	}
	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	// 如果当前Image正在被CPU提交数据，等待CPU
	if (m_imagesInFlight[imageIndex] != VK_NULL_HANDLE)
	{
		vkWaitForFences(m_backend->getDevice(), 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
	}
	m_imagesInFlight[imageIndex] = m_inFlightFences[m_currentFrame];

	// 更新Command Buffer
	updateCommandBuffer(imageIndex);

	// Submit command buffer
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(m_backend->getDevice(), 1, &m_inFlightFences[m_currentFrame]);
	if (vkQueueSubmit(m_backend->getGraphicsQueue(), 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	// Present
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapchains[] = { m_swapchain.get() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(m_backend->getPresentQueue(), &presentInfo);

	// 检测是否要重建交换链
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_backend->getFramebufferResized())
	{
		m_backend->setFramebufferResized(false);
		recreateSwapchain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::destroy()
{
	cleanupSwapchain();
	cleanupBatchResource();

	m_staticMeshPipeline.destroy();
	m_renderPass.destroy();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		vkDestroySemaphore(m_backend->getDevice(), m_renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(m_backend->getDevice(), m_imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(m_backend->getDevice(), m_inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(m_backend->getDevice(), m_commandPool, nullptr);
}

void Renderer::setBatchResources(std::vector<BatchResource*> batchResources)
{
	m_batchResources = batchResources;
	for (BatchResource* batchResource : m_batchResources)
	{
		m_staticMeshPipeline.createUniformBuffers(batchResource, sizeof(UBO));
		m_staticMeshPipeline.createDescriptorSets(batchResource);
	}
}

void Renderer::createCommandPool()
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = m_backend->getQueueFamilyIndices().graphicsFamily.value();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(m_backend->getDevice(), &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}
}

void Renderer::createMsaaResources()
{
	VkFormat msaaFormat = m_swapchain.getFormat();

	ResourceFactory::getInstance().createImage(m_swapchain.getExtent().width, m_swapchain.getExtent().height, 1, m_backend->getMsaaSamples(), msaaFormat,
		VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY, m_msaaImageView.vmaImage);
	m_msaaImageView.view = ResourceFactory::getInstance().createImageView(m_msaaImageView.vmaImage.image, msaaFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void Renderer::createDepthResources()
{
	ResourceFactory::getInstance().createImage(m_swapchain.getExtent().width, m_swapchain.getExtent().height, 1, m_backend->getMsaaSamples(), m_depthFormat, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VMA_MEMORY_USAGE_GPU_ONLY, m_depthImageView.vmaImage);
	m_depthImageView.view = ResourceFactory::getInstance().createImageView(m_depthImageView.vmaImage.image, m_depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

	ResourceFactory::getInstance().transitionImageLayout(m_depthImageView.vmaImage.image, m_depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
}

void Renderer::createFramebuffers()
{
	m_swapchainFramebuffers.resize(m_swapchain.getSize());

	for (size_t i = 0; i < m_swapchainFramebuffers.size(); ++i)
	{
		std::vector<VkImageView> attachments = { m_msaaImageView.view, m_depthImageView.view, m_swapchain.getImageView(i) };
		m_swapchainFramebuffers[i].init(m_backend, m_renderPass.get(), m_swapchain.getExtent(), attachments);
	}
}

void Renderer::createCommandBuffers()
{
	m_commandBuffers.resize(m_swapchain.getSize());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

	if (vkAllocateCommandBuffers(m_backend->getDevice(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

void Renderer::createSyncObjects()
{
	// 创建Semaphore信号量，用于GPU-GPU同步
	m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	// 创建Fence栅栏，用于CPU-GPU同步
	m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	// 将fence的初始状态设为SIGNALED，否则第一帧会永远在等待fence切换为SIGNALED状态
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	m_imagesInFlight.resize(m_swapchain.getSize(), VK_NULL_HANDLE);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		if (vkCreateSemaphore(m_backend->getDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_backend->getDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(m_backend->getDevice(), &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

void Renderer::recreateSwapchain()
{
	// 处理最小化的情况
	int width = 0;
	int height = 0;
	glfwGetFramebufferSize(m_backend->getWindow(), &width, &height);
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(m_backend->getWindow(), &width, &height);
		glfwWaitEvents();
	}

	// 等待所有的资源都处于闲置状态，不再被使用
	vkDeviceWaitIdle(m_backend->getDevice());

	// 然后再清理和创建交换链和依赖交换链的所有资源，不然会报错
	cleanupSwapchain();

	// 把和交换链相关的参数设置成动态状态，可以避免重建整个图形管线
	m_swapchain.init(m_backend);
	m_camera->setAspect(m_swapchain.getExtent().width / static_cast<float>(m_swapchain.getExtent().height));

	createMsaaResources();
	createDepthResources();
	createFramebuffers();
	createCommandBuffers();
}

void Renderer::cleanupSwapchain()
{
	m_msaaImageView.destroy(m_backend->getDevice(), m_backend->getAllocator());
	m_depthImageView.destroy(m_backend->getDevice(), m_backend->getAllocator());

	for (Framebufer& swapchainFramebuffer : m_swapchainFramebuffers)
	{
		swapchainFramebuffer.destroy();
	}

	m_swapchain.destroy();
}

void Renderer::cleanupBatchResource()
{
	for (BatchResource* batchResource : m_batchResources)
	{
		batchResource->destroy(m_backend->getDevice(), m_backend->getAllocator());
	}
}

void Renderer::updateCommandBuffer(uint32_t imageIndex)
{
	VkCommandBuffer commandBuffer = m_commandBuffers[imageIndex];
	vkResetCommandBuffer(commandBuffer, 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_renderPass.get();
	renderPassInfo.framebuffer = m_swapchainFramebuffers[imageIndex].get();
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_swapchain.getExtent();

	std::vector<VkClearValue> clearValues(2, VkClearValue{});
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());;
	renderPassInfo.pClearValues = clearValues.data();

	// Viewports and Scissors
	// Viewport定义了屏幕变换
	VkExtent2D swapchainExtent = m_swapchain.getExtent();
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapchainExtent.width);
	viewport.height = static_cast<float>(swapchainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	// Scissor定义了裁剪区域，被裁剪的部分会被栅格器丢弃，不进行fragment shader计算）
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapchainExtent;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_staticMeshPipeline.get());

	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	for (size_t j = 0; j < m_batchResources.size(); ++j)
	{
		BatchResource* batchResource = m_batchResources[j];

		VkBuffer vertexBuffers[] = { batchResource->vertexBuffer.buffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, batchResource->indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_staticMeshPipeline.getPipelineLayout(), 0, 1, &batchResource->descriptorSets[imageIndex], 0, nullptr);
		updatePushConstants(commandBuffer, j);

		vkCmdDrawIndexed(commandBuffer, batchResource->indiceSize, 1, 0, 0, 0);
	}

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void Renderer::updatePushConstants(VkCommandBuffer commandBuffer, size_t batchIndex)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	std::vector<glm::vec3> positions = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(-4.0f, -4.0f, 0.0f),
		glm::vec3(-4.0f, 4.0f, 0.0f),
		glm::vec3(4.0f, -4.0f, 0.0f),
		glm::vec3(4.0f, 4.0f, 0.0f)
	};

	VPCO vpco{};
	glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), positions[0]);
	vpco.mvp = m_camera->getViewPerspectiveMatrix();
	vpco.mvp *= modelMat;

	FPCO fpco{};
	fpco.cameraPosition = m_camera->getPosition();
	fpco.lightDirection = glm::vec3(-1.0f, 1.0f, -1.0f);

	const void* pcos[] = { &vpco, &fpco };
	const auto& pushConstantRanges = m_staticMeshPipeline.getPushConstantRanges();
	for (size_t i = 0; i < pushConstantRanges.size(); ++i)
	{
		const VkPushConstantRange& pushConstantRange = pushConstantRanges[i];
		vkCmdPushConstants(commandBuffer, m_staticMeshPipeline.getPipelineLayout(), pushConstantRange.stageFlags, pushConstantRange.offset, pushConstantRange.size, pcos[i]);
	}
}