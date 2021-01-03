#pragma once

#include "rendering/batch_resource.h"
#include "component/mesh_component.h"

class ResourceFactory
{
public:
	static ResourceFactory& getInstance();
	void init(class GraphicsBackend* graphicsBackend);
	void destroy();

	StaticMeshBatchResource* createBatchResource(const StaticMeshComponent& staticMeshComponent);

	void createVertexBuffer(const std::vector<Vertex>& vertices, VmaBuffer& vertexBuffer);
	void createIndexBuffer(const std::vector<uint32_t>& indices, VmaBuffer& indexBuffer, uint32_t& indiceSize);
	void createTextureImage(const Texture& texture, VmaImage& image);
	
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

	class GraphicsBackend* m_backend;
	VkCommandPool m_instantCommandPool;
};