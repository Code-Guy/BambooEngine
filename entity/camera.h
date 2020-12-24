#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera(glm::vec3 position, glm::vec3 up, 
		float yaw, float pitch,
		float speed, float sensitivity);

	void tick(float deltaTime);

	glm::mat4 getViewMatrix();

	void onKeyPressed(int key);
	void onMouseOffseted(float dx, float dy);

private:
	enum class Direction
	{
		Front, Back, Left, Right
	};

	void updatePose();

	glm::vec3 m_position;
	glm::vec3 m_up;
	float m_yaw;
	float m_pitch;

	float m_speed;
	float m_sensitivity;

	glm::vec3 m_forward;
	glm::vec3 m_right;

	Direction direction;
};