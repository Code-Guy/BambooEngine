#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#define SWAPCHAIN_IMAGE_NUM 3
#define INVALID_BONE -1
#define MAX_BONE_NUM 100
#define BONE_NUM_PER_VERTEX 4

enum class EPipelineType
{
	StaticMesh, SkeletalMesh
};

const glm::vec3 ForwardVector = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 RightVector = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 UpVector = glm::vec3(0.0f, 0.0f, 1.0f);

struct Transform
{
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);

	glm::mat4 matrix()
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

struct QuatTransform
{
	glm::vec3 position = glm::vec3(0.0f);
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
	
	glm::mat4 matrix()
	{
		glm::mat4 modelMatrix(1.0f);

		modelMatrix = glm::translate(modelMatrix, position);
		modelMatrix *= glm::mat4_cast(rotation);
		modelMatrix = glm::scale(modelMatrix, scale);

		return modelMatrix;
	}
};