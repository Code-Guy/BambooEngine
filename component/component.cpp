#include "component/component.h"
#include "rendering/resource_factory.h"

void StaticMeshComponent::initBatchResource()
{
	// 创建BasicBatchResource
	auto& factory = ResourceFactory::getInstance();
	auto basicBatchResource = std::make_shared<BasicBatchResource>();

	uint32_t bufferSize = static_cast<uint32_t>(sizeof(mesh->vertices[0]) * mesh->vertices.size());
	factory.createVertexBuffer(bufferSize, mesh->vertices.data(), basicBatchResource->vertexBuffer);
	factory.createIndexBuffer(mesh->indices, basicBatchResource->indexBuffer);

	basicBatchResource->indexCounts.resize(sections.size());
	basicBatchResource->baseIVSs.resize(sections.size());

	for (size_t i = 0; i < sections.size(); ++i)
	{
		const Section& section = sections[i];
		VmaImageViewSampler& baseIVS = basicBatchResource->baseIVSs[i];

		basicBatchResource->indexCounts[i] = section.indexCount;

		VmaImage& vmaImage = baseIVS.vmaImage;
		factory.createTextureImage(section.material->baseTex, vmaImage);
		baseIVS.view = factory.createImageView(vmaImage.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, vmaImage.mipLevels);
		baseIVS.sampler = factory.createSampler(VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, vmaImage.mipLevels);
	}

	basicBatchResource->uniformBuffers.resize(SWAPCHAIN_IMAGE_NUM);
	for (size_t i = 0; i < SWAPCHAIN_IMAGE_NUM; ++i)
	{
		factory.createBuffer(sizeof(StaticMeshUBO),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VMA_MEMORY_USAGE_CPU_ONLY,
			basicBatchResource->uniformBuffers[i]);
	}

	// 注册到StaticMeshPipeline里
	batchResource = basicBatchResource;
	factory.registerBatchResource(EPipelineType::StaticMesh, batchResource);
}

void StaticMeshComponent::destroyBatchResource()
{
	ResourceFactory::getInstance().unregisterBatchResource(EPipelineType::StaticMesh, batchResource);
}


void SkeletalMeshComponent::initBatchResource()
{
	// 创建BasicBatchResource
	auto& factory = ResourceFactory::getInstance();
	auto basicBatchResource = std::make_shared<BasicBatchResource>();

	uint32_t bufferSize = static_cast<uint32_t>(sizeof(mesh->vertices[0]) * mesh->vertices.size());
	factory.createVertexBuffer(bufferSize, mesh->vertices.data(), basicBatchResource->vertexBuffer);
	factory.createIndexBuffer(mesh->indices, basicBatchResource->indexBuffer);

	basicBatchResource->indexCounts.resize(sections.size());
	basicBatchResource->baseIVSs.resize(sections.size());

	for (size_t i = 0; i < sections.size(); ++i)
	{
		const Section& section = sections[i];
		VmaImageViewSampler& baseIVS = basicBatchResource->baseIVSs[i];

		basicBatchResource->indexCounts[i] = section.indexCount;

		VmaImage& vmaImage = baseIVS.vmaImage;
		factory.createTextureImage(section.material->baseTex, vmaImage);
		baseIVS.view = factory.createImageView(vmaImage.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, vmaImage.mipLevels);
		baseIVS.sampler = factory.createSampler(VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, vmaImage.mipLevels);
	}

	basicBatchResource->uniformBuffers.resize(SWAPCHAIN_IMAGE_NUM);
	for (size_t i = 0; i < SWAPCHAIN_IMAGE_NUM; ++i)
	{
		factory.createBuffer(sizeof(StaticMeshUBO),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VMA_MEMORY_USAGE_CPU_ONLY,
			basicBatchResource->uniformBuffers[i]);
	}

	// 注册到StaticMeshPipeline里
	batchResource = basicBatchResource;
	factory.registerBatchResource(EPipelineType::SkeletalMesh, batchResource);
}

void SkeletalMeshComponent::destroyBatchResource()
{
	ResourceFactory::getInstance().unregisterBatchResource(EPipelineType::SkeletalMesh, batchResource);
}
