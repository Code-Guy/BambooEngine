#include "component/component.h"
#include "rendering/resource_factory.h"

void StaticMeshComponent::initBatchResource()
{
	// 创建StaticMeshBatchResource
	auto& factory = ResourceFactory::getInstance();
	auto staticBatchResource = std::make_shared<StaticMeshBatchResource>();

	factory.createVertexBuffer(mesh->vertices, staticBatchResource->vertexBuffer);
	factory.createIndexBuffer(mesh->indices, staticBatchResource->indexBuffer);

	staticBatchResource->indexCounts.resize(sections.size());
	staticBatchResource->baseIVSs.resize(sections.size());

	for (size_t i = 0; i < sections.size(); ++i)
	{
		const Section& section = sections[i];
		VmaImageViewSampler& baseIVS = staticBatchResource->baseIVSs[i];

		staticBatchResource->indexCounts[i] = section.indexCount;

		VmaImage& vmaImage = baseIVS.vmaImage;
		factory.createTextureImage(section.material->baseTex, vmaImage);
		baseIVS.view = factory.createImageView(vmaImage.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, vmaImage.mipLevels);
		baseIVS.sampler = factory.createSampler(VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, vmaImage.mipLevels);
	}

	staticBatchResource->uniformBuffers.resize(SWAPCHAIN_IMAGE_NUM);
	for (size_t i = 0; i < SWAPCHAIN_IMAGE_NUM; ++i)
	{
		factory.createBuffer(sizeof(UBO),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VMA_MEMORY_USAGE_CPU_ONLY,
			staticBatchResource->uniformBuffers[i]);
	}

	// 注册到StaticMeshPipeline里
	batchResource = staticBatchResource;
	factory.registerBatchResource(batchResource);
}

void StaticMeshComponent::destroyBatchResource()
{
	ResourceFactory::getInstance().unregisterBatchResource(batchResource);
}


void SkinnedMeshComponent::initBatchResource()
{

}

void SkinnedMeshComponent::destroyBatchResource()
{

}
