#pragma once

#include <vulkan/vulkan.h>
#include <set>

#include "graphics_backend.h"
#include "io/asset_loader.h"
#include "resource_factory.h"

class Pipeline
{
public:
	void init(std::shared_ptr<GraphicsBackend> backend, VkRenderPass renderPass);
	void destroy();

	VkPipeline get() { return m_pipeline; }
	VkPipelineLayout getPipelineLayout() { return m_pipelineLayout; }

	std::set<std::shared_ptr<BatchResource>>& getBatchResources() { return m_batchResources; }
	void registerBatchResource(std::shared_ptr<BatchResource> batchResource);
	void unregisterBatchResource(std::shared_ptr<BatchResource> batchResource);

	virtual void pushConstants(VkCommandBuffer commandBuffer, std::shared_ptr<BatchResource> batchResource) = 0;

protected:
	virtual uint32_t getMaxBatchNum() = 0;
	virtual void createDescriptorSetLayout() = 0;
	virtual void createDescriptorPool() = 0;
	virtual std::vector<VkPipelineShaderStageCreateInfo> createShaderStages(std::vector<VkShaderModule>& shaderModules) = 0;
	virtual VkPipelineVertexInputStateCreateInfo createVertexInputState() = 0;
	virtual std::vector<VkPushConstantRange> createPushConstantRanges() = 0;

	virtual void createDescriptorSets(std::shared_ptr<BatchResource> batchResource) = 0;

	std::shared_ptr<GraphicsBackend> m_backend;
	VkRenderPass m_renderPass;

	VkDescriptorSetLayout m_descriptorSetLayout;
	VkDescriptorPool m_descriptorPool;
	VkPipelineLayout m_pipelineLayout;
	VkPipeline m_pipeline;

	std::vector<VkVertexInputBindingDescription> m_bindingDescriptions; // ∂•µ„∞Û∂®√Ë ˆ
	std::vector<VkVertexInputAttributeDescription> m_attributeDescriptions; // ∂•µ„ Ù–‘√Ë ˆ

	std::vector<VkPushConstantRange> m_pushConstantRanges;

private:
	void createPipeline();

	std::set<std::shared_ptr<BatchResource>> m_batchResources;
};