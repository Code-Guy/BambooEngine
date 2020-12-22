#pragma once

#include <vulkan/vulkan.h>
#include <vulkan_memory_allocator/vk_mem_alloc.h>
#include <glm/glm.hpp>

#include <vector>

struct UniformBufferObject
{
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
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
};

struct VmaImageView
{
	VkImage image;
	VmaAllocation allocation;
	VkImageView imageView;
};

struct VmaImageViewSampler
{
	VkImage image;
	VmaAllocation allocation;
	VkImageView imageView;
	VkSampler sampler;
};

struct BatchResource 
{
	VmaBuffer vertexBuffer;
	VmaBuffer indexBuffer;
	VmaImageViewSampler textureImageViewSampler;

	std::vector<VmaBuffer> uniformBuffers;
	std::vector<VkDescriptorSet> descriptorSets;
};