#pragma once

#include <vulkan/vulkan.h>
#include <vulkan_memory_allocator/vk_mem_alloc.h>
#include <glm/glm.hpp>

#include <vector>

struct UBO
{
	glm::mat4 mvp;
};

struct VPCO
{
	glm::mat4 mvp;
};

struct FPCO
{
	glm::vec3 cameraPosition; float p0;
	glm::vec3 lightDirection; float p1;
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