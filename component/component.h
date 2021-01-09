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
const glm::vec3 ForwardVector = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 RightVector = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 UpVector = glm::vec3(0.0f, 0.0f, 1.0f);

struct TransformComponent : public Component
{
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);

	glm::mat4 localMatrix = glm::mat4(1.0f);
	glm::mat4 worldMatrix = glm::mat4(1.0f);

	virtual bool isValid() override
	{
		return true;
	}

	glm::mat4 calcModelMatrix()
	{
		glm::mat4 modelMatrix(1.0f);

		modelMatrix = glm::translate(modelMatrix, position);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), ForwardVector);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), RightVector);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), UpVector);
		modelMatrix = glm::scale(modelMatrix, scale);

		return modelMatrix;
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
class AnimatorComponent : public Component
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

	void play(const std::string& name, bool loop = true);
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