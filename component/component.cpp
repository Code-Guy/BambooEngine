#include "component/component.h"
#include "rendering/resource_factory.h"
#include "rendering/renderer.h"

void MeshComponent::updateUniformBuffer(std::shared_ptr<class Renderer> renderer, size_t bufferSize, void* bufferData)
{
	void* data;
	VmaAllocation uniformBufferAllocation = batchResource->uniformBuffers[renderer->getImageIndex()].allocation;
	vmaMapMemory(renderer->getBackend()->getAllocator(), uniformBufferAllocation, &data);
	memcpy(data, bufferData, bufferSize);
	vmaUnmapMemory(renderer->getBackend()->getAllocator(), uniformBufferAllocation);
}

void StaticMeshComponent::initBatchResource(std::shared_ptr<class Renderer> renderer)
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
	renderer->getPipeline(EPipelineType::StaticMesh)->registerBatchResource(batchResource);
}

void StaticMeshComponent::destroyBatchResource(std::shared_ptr<class Renderer> renderer)
{
	batchResource->destroy(renderer->getBackend()->getDevice(), renderer->getBackend()->getAllocator());
	renderer->getPipeline(EPipelineType::StaticMesh)->unregisterBatchResource(batchResource);
}


void SkeletalMeshComponent::initBatchResource(std::shared_ptr<class Renderer> renderer)
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
		factory.createBuffer(sizeof(SkeletalMeshUBO),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VMA_MEMORY_USAGE_CPU_ONLY,
			basicBatchResource->uniformBuffers[i]);
	}

	// 注册到StaticMeshPipeline里
	batchResource = basicBatchResource;
	renderer->getPipeline(EPipelineType::SkeletalMesh)->registerBatchResource(batchResource);
}

void SkeletalMeshComponent::destroyBatchResource(std::shared_ptr<class Renderer> renderer)
{
	batchResource->destroy(renderer->getBackend()->getDevice(), renderer->getBackend()->getAllocator());
	renderer->getPipeline(EPipelineType::SkeletalMesh)->unregisterBatchResource(batchResource);
}


AnimatorComponent::AnimatorComponent()
{
	m_time = 0.0f;
	m_loop = false;
	m_playing = false;
	m_paused = false;
	m_name.clear();
}

bool AnimatorComponent::isCompatible(std::shared_ptr<Skeleton> skeleton)
{
	for (auto& iter : animations)
	{
		if (!iter.second->isCompatible(skeleton))
		{
			return false;
		}
	}
	return true;
}

void AnimatorComponent::merge(const AnimatorComponent& other)
{
	animations.insert(other.animations.begin(), other.animations.end());
}

void AnimatorComponent::tick(float deltaTime)
{
	if (!m_playing || m_paused)
	{
		return;
	}

	m_time += deltaTime;
}

void AnimatorComponent::play(const std::string& name, bool loop)
{
	m_name = name;
	m_loop = loop;
	m_playing = true;
	m_time = 0.0f;
}

void AnimatorComponent::replay()
{
	m_paused = false;
}

void AnimatorComponent::pause()
{
	m_paused = true;
}

void AnimatorComponent::stop()
{
	m_paused = true;
	m_playing = false;
	m_time = 0.0f;
}
