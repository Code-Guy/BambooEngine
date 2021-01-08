#pragma once

#include <vulkan/vulkan.h>
#include <vulkan_memory_allocator/vk_mem_alloc.h>
#include <glm/glm.hpp>
#include <vector>

#include "core/engine_type.h"

struct StaticMeshUBO
{
	glm::mat4 padding;
};

struct SkeletalMeshUBO
{
	glm::mat4 padding;
};

struct VPCO
{
	glm::mat4 m;
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

	void destroy(VmaAllocator allocator)
	{
		vmaDestroyBuffer(allocator, buffer, allocation);
	}
};

struct VmaImage
{
	VkImage image;
	VmaAllocation allocation;
	uint32_t mipLevels;

	void destroy(VmaAllocator allocator)
	{
		vmaDestroyImage(allocator, image, allocation);
	}
};

struct VmaImageView
{
	VmaImage vmaImage;
	VkImageView view;

	void destroy(VkDevice device, VmaAllocator allocator)
	{
		vkDestroyImageView(device, view, nullptr);
		vmaImage.destroy(allocator);
	}
};

struct VmaImageViewSampler
{
	VmaImage vmaImage;
	VkImageView view;
	VkSampler sampler;

	void destroy(VkDevice device, VmaAllocator allocator)
	{
		vkDestroySampler(device, sampler, nullptr);
		vkDestroyImageView(device, view, nullptr);
		vmaImage.destroy(allocator);
	}
};

struct BatchResource 
{
	VmaBuffer vertexBuffer;
	VmaBuffer indexBuffer;
	std::vector<uint32_t> indexCounts;

	std::vector<VmaBuffer> uniformBuffers;
	std::vector<VkDescriptorSet> descriptorSets;

	virtual void destroy(VkDevice device, VmaAllocator allocator)
	{
		for (VmaBuffer& uniformBuffer : uniformBuffers)
		{
			uniformBuffer.destroy(allocator);
		}

		indexBuffer.destroy(allocator);
		vertexBuffer.destroy(allocator);
	}
};

struct BasicBatchResource : public BatchResource
{
	std::vector<VmaImageViewSampler> baseIVSs;

	// push constants
	VPCO vpco;
	FPCO fpco;

	virtual void destroy(VkDevice device, VmaAllocator allocator)
	{
		for (VmaImageViewSampler& baseIVS : baseIVSs)
		{
			baseIVS.destroy(device, allocator);
		}
		BatchResource::destroy(device, allocator);
	}
};