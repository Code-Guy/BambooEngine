#include "static_mesh_component.h"
#include "rendering/resource_factory.h"

void StaticMeshComponent::pre()
{
	MeshComponent::pre();

	// 创建StaticMeshBatchResource
	auto& factory = ResourceFactory::getInstance();
	auto batchResource = std::make_shared<StaticMeshBatchResource>();

	factory.createVertexBuffer(m_mesh->vertices, batchResource->vertexBuffer);
	factory.createIndexBuffer(m_mesh->indices, batchResource->indexBuffer);

	batchResource->indexCounts.resize(m_sections.size());
	batchResource->baseIVSs.resize(m_sections.size());

	for (size_t i = 0; i < m_sections.size(); ++i)
	{
		const Section& section = m_sections[i];
		VmaImageViewSampler& baseIVS = batchResource->baseIVSs[i];

		batchResource->indexCounts[i] = section.indexCount;

		VmaImage& vmaImage = baseIVS.vmaImage;
		factory.createTextureImage(section.material->baseTex, vmaImage);
		baseIVS.view = factory.createImageView(vmaImage.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, vmaImage.mipLevels);
		baseIVS.sampler = factory.createSampler(VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, vmaImage.mipLevels);
	}

	batchResource->uniformBuffers.resize(SWAPCHAIN_IMAGE_NUM);
	for (size_t i = 0; i < SWAPCHAIN_IMAGE_NUM; ++i)
	{
		factory.createBuffer(sizeof(UBO),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VMA_MEMORY_USAGE_CPU_ONLY,
			batchResource->uniformBuffers[i]);
	}

	// 注册到StaticMeshPipeline里
	m_batchResource = batchResource;
	factory.registerBatchResource(m_batchResource);
}

void StaticMeshComponent::tick(float deltaTime)
{
	MeshComponent::tick(deltaTime);
}

void StaticMeshComponent::post()
{
	MeshComponent::post();

	ResourceFactory::getInstance().unregisterBatchResource(m_batchResource);
}