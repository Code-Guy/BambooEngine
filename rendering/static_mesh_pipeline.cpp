#include "static_mesh_pipeline.h"

void StaticMeshPipeline::createDescriptorSets(std::shared_ptr<BatchResource> batchResource)
{
	StaticMeshBatchResource* batch = (StaticMeshBatchResource*)batchResource.get();
	uint32_t sectionCount = static_cast<uint32_t>(batch->indexCounts.size());
	uint32_t descriptorSetSize = SWAPCHAIN_IMAGE_NUM * sectionCount;

	std::vector<VkDescriptorSetLayout> layouts(descriptorSetSize, m_descriptorSetLayout);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
	allocInfo.pSetLayouts = layouts.data();

	batch->descriptorSets.resize(descriptorSetSize);
	if (vkAllocateDescriptorSets(m_backend->getDevice(), &allocInfo, batch->descriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < SWAPCHAIN_IMAGE_NUM; ++i)
	{
		for (size_t j = 0; j < sectionCount; ++j)
		{
			size_t index = sectionCount * i + j;

			std::vector<VkWriteDescriptorSet> descriptorWrites(2, VkWriteDescriptorSet{});

			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = batch->uniformBuffers[i].buffer;
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UBO);

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = batch->descriptorSets[index];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = batch->baseIVSs[j].view;
			imageInfo.sampler = batch->baseIVSs[j].sampler;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = batch->descriptorSets[index];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(m_backend->getDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}
}

void StaticMeshPipeline::pushConstants(VkCommandBuffer commandBuffer, std::shared_ptr<BatchResource> batchResource)
{
	StaticMeshBatchResource* batch = (StaticMeshBatchResource*)batchResource.get();

	const void* pcos[] = { &batch->vpco, &batch->fpco };
	for (size_t i = 0; i < m_pushConstantRanges.size(); ++i)
	{
		const VkPushConstantRange& pushConstantRange = m_pushConstantRanges[i];
		vkCmdPushConstants(commandBuffer, m_pipelineLayout, pushConstantRange.stageFlags, pushConstantRange.offset, pushConstantRange.size, pcos[i]);
	}
}

uint32_t StaticMeshPipeline::getMaxBatchNum()
{
	return 32;
}

void StaticMeshPipeline::createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerLayoutBinding.pImmutableSamplers = nullptr;

	std::vector<VkDescriptorSetLayoutBinding> bindings = { uboLayoutBinding, samplerLayoutBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(m_backend->getDevice(), &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void StaticMeshPipeline::createDescriptorPool()
{
	uint32_t descriptorCount = SWAPCHAIN_IMAGE_NUM * getMaxBatchNum();

	std::vector<VkDescriptorPoolSize> poolSizes(2, VkDescriptorPoolSize{});
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(descriptorCount);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(descriptorCount);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(descriptorCount);

	if (vkCreateDescriptorPool(m_backend->getDevice(), &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

std::vector<VkPipelineShaderStageCreateInfo> StaticMeshPipeline::createShaderStages(std::vector<VkShaderModule>& shaderModules)
{
	// 加载shader binary code
	std::vector<char> vertShaderCode = AssetLoader::getInstance().loadBinary("asset/shader/spv/blinn_phong_vert.spv");
	std::vector<char> fragShaderCode = AssetLoader::getInstance().loadBinary("asset/shader/spv/blinn_phong_frag.spv");
	VkShaderModule vertShaderModule = ResourceFactory::getInstance().createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = ResourceFactory::getInstance().createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";
	vertShaderStageInfo.pSpecializationInfo = nullptr; // 定义着色器常量，可以编译期优化

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";
	fragShaderStageInfo.pSpecializationInfo = nullptr;

	shaderModules = { vertShaderModule, fragShaderModule };
	return { vertShaderStageInfo, fragShaderStageInfo };
}

VkPipelineVertexInputStateCreateInfo StaticMeshPipeline::createVertexInputState()
{
	// StaticVertex Input
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	// 顶点绑定描述
	m_bindingDescriptions.resize(1, VkVertexInputBindingDescription{});
	m_bindingDescriptions[0].binding = 0;
	m_bindingDescriptions[0].stride = sizeof(StaticVertex);
	m_bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	// 顶点属性描述
	m_attributeDescriptions.resize(3, VkVertexInputAttributeDescription{});

	m_attributeDescriptions[0].binding = 0;
	m_attributeDescriptions[0].location = 0;
	m_attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	m_attributeDescriptions[0].offset = offsetof(StaticVertex, position);

	m_attributeDescriptions[1].binding = 0;
	m_attributeDescriptions[1].location = 1;
	m_attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
	m_attributeDescriptions[1].offset = offsetof(StaticVertex, texCoord);

	m_attributeDescriptions[2].binding = 0;
	m_attributeDescriptions[2].location = 2;
	m_attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
	m_attributeDescriptions[2].offset = offsetof(StaticVertex, normal);

	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(m_bindingDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = m_bindingDescriptions.data();
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = m_attributeDescriptions.data();

	return vertexInputInfo;
}

std::vector<VkPushConstantRange> StaticMeshPipeline::createPushConstantRanges()
{
	std::vector<VkPushConstantRange> pushConstantRanges(2, VkPushConstantRange{});

	pushConstantRanges[0].offset = 0;
	pushConstantRanges[0].size = sizeof(VPCO);
	pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	pushConstantRanges[1].offset = pushConstantRanges[0].size;
	pushConstantRanges[1].size = sizeof(FPCO);
	pushConstantRanges[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	return pushConstantRanges;
}
