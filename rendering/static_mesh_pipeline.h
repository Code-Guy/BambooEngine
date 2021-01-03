#pragma once

#include "pipeline.h"

class StaticMeshPipeline : public Pipeline
{
public:
	virtual void createDescriptorSets(BatchResource* batchResource);

protected:
	virtual uint32_t getMaxBatchNum();
	virtual void createDescriptorSetLayout();
	virtual void createDescriptorPool();
	virtual std::vector<VkPipelineShaderStageCreateInfo> createShaderStages(std::vector<VkShaderModule>& shaderModules);
	virtual VkPipelineVertexInputStateCreateInfo createVertexInputState();
	virtual std::vector<VkPushConstantRange> createPushConstantRanges();

private:

};