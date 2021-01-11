#pragma once

#include "material.h"
#include "mesh.h"
#include "animation.h"
#include "rendering/batch_resource.h"

/* Base */
struct Component
{
	virtual bool isValid() = 0;
};

/* Tag */
struct TagComponent : public Component
{
	std::string name;

	virtual bool isValid() override
	{
		return !name.empty();
	}
};

/* Transform */
struct TransformComponent : public Transform, public Component
{
	glm::mat4 localMatrix = glm::mat4(1.0f);
	glm::mat4 worldMatrix = glm::mat4(1.0f);

	virtual bool isValid() override
	{
		return true;
	}
};

/* Mesh */
struct Section
{
	std::shared_ptr<Material> material;
	uint32_t indexCount;
};

struct MeshComponent : public Component
{
	virtual bool isValid() override
	{
		return !sections.empty();
	}

	virtual void initBatchResource(std::shared_ptr<class Renderer> renderer) = 0;
	virtual void destroyBatchResource(std::shared_ptr<class Renderer> renderer) = 0;
	virtual void updateUniformBuffer(std::shared_ptr<class Renderer> renderer, size_t bufferSize, void* bufferData);

	std::vector<Section> sections;
	std::shared_ptr<BasicBatchResource> batchResource;
};

/* Static Mesh */
struct StaticMeshComponent : public MeshComponent
{
	virtual void initBatchResource(std::shared_ptr<class Renderer> renderer) override;
	virtual void destroyBatchResource(std::shared_ptr<class Renderer> renderer) override;

	std::shared_ptr<StaticMesh> mesh;
};

/* Skeletal Mesh */
struct SkeletalMeshComponent : public MeshComponent
{
	virtual void initBatchResource(std::shared_ptr<class Renderer> renderer) override;
	virtual void destroyBatchResource(std::shared_ptr<class Renderer> renderer) override;

	std::shared_ptr<Skeleton> skeleton;
	std::shared_ptr<SkeletalMesh> mesh;
};

/* Animator */
struct AnimatorComponent : public Component
{
public:
	AnimatorComponent();

	virtual bool isValid() override
	{
		return !animations.empty();
	}

	bool isCompatible(std::shared_ptr<Skeleton> skeleton);
	void merge(const AnimatorComponent& other);
	void tick(float deltaTime);

	void play(const std::string& name = "", bool loop = true);
	void replay();
	void pause();
	void stop();

	std::shared_ptr<Skeleton> skeleton;
	std::map<std::string, std::shared_ptr<Animation>> animations;
	glm::mat4 gBones[MAX_BONE_NUM];

private:
	float m_time;
	bool m_loop;
	bool m_playing;
	bool m_paused;
	std::string m_name;
};