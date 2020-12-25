#pragma once

#include <vulkan/vulkan.h>
#include <vulkan_memory_allocator/vk_mem_alloc.h>
#include <glm/glm.hpp>

#include <vector>

struct UniformBufferObject
{
	alignas(16) glm::mat4 mvp;
};

struct PushConstantsObject
{
	alignas(16) glm::mat4 mvp;
	alignas(16) glm::vec4 cameraPosition;
	alignas(16) glm::vec4 lightDirection;
};

struct VmaBuffer
{
	VkBuffer buffer;
	VmaAllocation allocation;
};

struct VmaImage
{
	VkImage image;
	VmaAllocation allocation;
	uint32_t mipLevels;
};

struct VmaImageView
{
	VmaImage vmaImage;
	VkImageView view;
};

struct VmaImageViewSampler
{
	VmaImage vmaImage;
	VkImageView view;
	VkSampler sampler;
};

struct BatchResource 
{
	VmaBuffer vertexBuffer;
	VmaBuffer indexBuffer;
	uint32_t indiceSize;
	VmaImageViewSampler baseIVS;

	std::vector<VmaBuffer> uniformBuffers;
	std::vector<VkDescriptorSet> descriptorSets;
};