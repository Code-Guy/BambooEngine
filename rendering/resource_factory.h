#pragma once

#include "rendering/batch_resource.h"
#include "component/material.h"

class ResourceFactory
{
public:
	static ResourceFactory& getInstance();
	void init(std::shared_ptr<class GraphicsBackend>& backend);
	void destroy();

	void createVertexBuffer(uint32_t bufferSize, void* verticesData, VmaBuffer& vertexBuffer);
	void createIndexBuffer(const std::vector<uint32_t>& indices, VmaBuffer& indexBuffer);
	void createTextureImage(std::shared_ptr<Texture>& texture, VmaImage& image);
	
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	VkSampler createSampler(VkFilter minFilter, VkFilter maxFilter, VkSamplerAddressMode adressMode, uint32_t mipLevels);

	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
		VkFormat format, VkImageTiling tiling, VkImageUsageFlags imageUsage, VmaMemoryUsage memoryUsage, VmaImage& image);
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage, VmaBuffer& buffer);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	VkShaderModule createShaderModule(const std::vector<char>& shaderCode);

	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

private:
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void generateMipmaps(VkImage image, VkFormat imageFormat, uint32_t width, uint32_t height, uint32_t mipLevels);

	void createInstantCommandPool();
	VkCommandBuffer beginInstantCommands();
	void endInstantCommands(VkCommandBuffer commandBuffer);

	std::shared_ptr<class GraphicsBackend> m_backend;
	VkCommandPool m_instantCommandPool;
};