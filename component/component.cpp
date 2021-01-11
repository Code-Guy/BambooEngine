#include "component/component.h"
#include "rendering/resource_factory.h"
#include "rendering/renderer.h"
#include <algorithm>

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

	if (animations.find(m_name) != animations.end())
	{
		auto& animation = animations[m_name];
		for (auto& iter : animation->positionKeys)
		{
			const std::string& name = iter.first;
			const std::vector<VectorKey>& positions = iter.second;

			VectorKey val{ m_time };
			const VectorKey& low = *std::lower_bound(positions.begin(), positions.end(), val);
			const VectorKey& up = *std::upper_bound(positions.begin(), positions.end(), val);

			float t = (m_time - low.time) / (up.time - low.time);
			Bone& bone = skeleton->getBone(name);
			bone.transform.position = glm::mix(low.value, up.value, t);
		}

		for (auto& iter : animation->rotationKeys)
		{
			const std::string& name = iter.first;
			const std::vector<QuatKey>& rotations = iter.second;

			QuatKey val{ m_time };
			const QuatKey& low = *std::lower_bound(rotations.begin(), rotations.end(), val);
			const QuatKey& up = *std::upper_bound(rotations.begin(), rotations.end(), val);

			float t = (m_time - low.time) / (up.time - low.time);
			Bone& bone = skeleton->getBone(name);
			bone.transform.rotation = glm::eulerAngles(glm::slerp(low.value, up.value, t));
		}

		for (auto& iter : animation->scaleKeys)
		{
			const std::string& name = iter.first;
			const std::vector<VectorKey>& scales = iter.second;

			VectorKey val{ m_time };
			const VectorKey& low = *std::lower_bound(scales.begin(), scales.end(), val);
			const VectorKey& up = *std::upper_bound(scales.begin(), scales.end(), val);

			float t = (m_time - low.time) / (up.time - low.time);
			Bone& bone = skeleton->getBone(name);
			bone.transform.scale = glm::mix(low.value, up.value, t);
		}

		m_time += deltaTime;
	}

	skeleton->update();
}

void AnimatorComponent::play(const std::string& name, bool loop)
{
	if (animations.empty())
	{
		return;
	}

	m_name = name;
	if (name.empty())
	{
		m_name = animations.begin()->first;
	}

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
