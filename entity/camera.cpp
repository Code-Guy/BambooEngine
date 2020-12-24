#include "camera.h"
#include <algorithm>
#include <GLFW/glfw3.h>
#include <cstdio>

Camera::Camera(glm::vec3 position, glm::vec3 up,
	float yaw, float pitch, 
	float speed, float sensitivity) :
	m_position(position), m_up(up),
	m_yaw(yaw), m_pitch(pitch),
	m_speed(speed), m_sensitivity(sensitivity)
{

}

void Camera::tick(float deltaTime)
{
	float offset = m_speed * deltaTime;

	switch (direction)
	{
	case Camera::Direction::Front:
		m_position += m_forward * offset;
		break;
	case Camera::Direction::Back:
		m_position -= m_forward * offset;
		break;
	case Camera::Direction::Left:
		m_position -= m_right * offset;
		break;
	case Camera::Direction::Right:
		m_position += m_right * offset;
		break;
	default:
		break;
	}
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(m_position, m_position + m_forward, m_up);
}

void Camera::onKeyPressed(int key)
{
	if (key == GLFW_KEY_W)
	{
		direction = Direction::Front;
	}
	else if (key == GLFW_KEY_S)
	{
		direction = Direction::Back;
	}
	else if (key == GLFW_KEY_A)
	{
		direction = Direction::Left;
	}
	else if (key == GLFW_KEY_D)
	{
		direction = Direction::Right;
	}
}

void Camera::onMouseOffseted(float dx, float dy)
{
	dx *= m_sensitivity;
	dy *= m_sensitivity;

	m_yaw += dx;
	m_pitch += dy;
	m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

	printf("dx: %f, dy: %f\n", dx, dy);

	updatePose();
}

void Camera::updatePose()
{
	m_forward.x = std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
	m_forward.y = std::sin(glm::radians(m_pitch));
	m_forward.z = std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
	m_forward = glm::normalize(m_forward);

	m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_up = glm::normalize(glm::cross(m_right, m_forward));
}
