#pragma once

#include "pipeline.h"

class SkeletalMeshPipeline : public Pipeline
{
public:
	virtual void pushConstants(VkCommandBuffer commandBuffer, std::shared_ptr<BatchResource> batchResource);

protected:
	virtual uint32_t getMaxBatchNum();
	virtual void createDescriptorSetLayout();
	virtual void createDescriptorPool();
	virtual std::vector<VkPipelineShaderStageCreateInfo> createShaderStages(std::vector<VkShaderModule>& shaderModules);
	virtual VkPipelineVertexInputStateCreateInfo createVertexInputState();
	virtual std::vector<VkPushConstantRange> createPushConstantRanges();

	virtual void createDescriptorSets(std::shared_ptr<BatchResource> batchResource);

private:

};