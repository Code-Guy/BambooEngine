#pragma once

#include "material.h"
#include "mesh.h"
#include "rendering/batch_resource.h"
#include <glm/gtc/matrix_transform.hpp>

/* Tag */
struct TagComponent
{
	std::string m_name;
};

/* Transform */
const glm::vec3 ForwardVector = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 RightVector = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 UpVector = glm::vec3(0.0f, 0.0f, 1.0f);

struct TransformComponent
{
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);

	glm::mat4 localMatrix = glm::mat4(1.0f);
	glm::mat4 worldMatrix = glm::mat4(1.0f);

	glm::mat4 calcModelMatrix()
	{
		glm::mat4 modelMatrix(1.0f);

		glm::scale(modelMatrix, scale);
		glm::rotate(modelMatrix, rotation.z, UpVector);
		glm::rotate(modelMatrix, rotation.y, RightVector);
		glm::rotate(modelMatrix, rotation.x, ForwardVector);
		glm::translate(modelMatrix, position);

		return modelMatrix;
	}
};

/* Mesh */
struct Section
{
	std::shared_ptr<Material> material;
	uint32_t indexCount;
};

struct MeshComponent
{
	virtual void initBatchResource() = 0;
	virtual void destroyBatchResource() = 0;

	std::vector<Section> sections;
};

/* Static Mesh */
struct StaticMeshComponent : public MeshComponent
{
public:
	virtual void initBatchResource() override;
	virtual void destroyBatchResource() override;

	std::shared_ptr<StaticMesh> mesh;
	std::shared_ptr<StaticMeshBatchResource> batchResource;
};

/* Skinned Mesh */
struct SkinnedMeshComponent : public MeshComponent
{
public:
	virtual void initBatchResource() override;
	virtual void destroyBatchResource() override;

	std::shared_ptr<SkinnedMesh> mesh;
};