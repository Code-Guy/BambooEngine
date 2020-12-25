#include "camera.h"
#include <algorithm>
#include <GLFW/glfw3.h>
#include <cstdio>

Camera::Camera(glm::vec3 position,
	float yaw, float pitch, 
	float speed, float sensitivity) :
	m_position(position),
	m_yaw(yaw), m_pitch(pitch),
	m_speed(speed), m_sensitivity(sensitivity)
{
	m_moveForward = false;
	m_moveBack = false;
	m_moveLeft = false;
	m_moveRight = false;

	m_mouseRightButtonPressed = false;

	updatePose();
}

void Camera::setFovy(float fovy)
{
	m_fovy = fovy;
}

void Camera::setAspect(float aspect)
{
	m_aspect = aspect;
}

void Camera::setClipping(float zNear, float zFar)
{
	m_zNear = zNear;
	m_zFar = zFar;
}

void Camera::tick(float deltaTime)
{
	float offset = m_speed * deltaTime;

	if (m_moveForward)
	{
		m_position += m_forward * offset;
	}
	if (m_moveBack)
	{
		m_position -= m_forward * offset;
	}
	if (m_moveLeft)
	{
		m_position -= m_right * offset;
	}
	if (m_moveRight)
	{
		m_position += m_right * offset;
	}
}

glm::vec3 Camera::getPosition()
{
	return m_position;
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(m_position, m_position + m_forward, m_up);
}

glm::mat4 Camera::getPerspectiveMatrix()
{
	glm::mat4 projMat = glm::perspective(glm::radians(m_fovy), m_aspect, m_zNear, m_zFar);
	projMat[1][1] *= -1.0f; // glm主要是为OpenGL设计的，OpenGL和Vulkan的坐标系Y轴朝向相反，因此这里要乘以-1

	return projMat;
}

glm::mat4 Camera::getViewPerspectiveMatrix()
{
	return getPerspectiveMatrix() * getViewMatrix();
}

void Camera::onKeyPressed(int key)
{
	if (key == GLFW_KEY_W)
	{
		m_moveForward = true;
	}
	if (key == GLFW_KEY_S)
	{
		m_moveBack = true;
	}
	if (key == GLFW_KEY_A)
	{
		m_moveLeft = true;
	}
	if (key == GLFW_KEY_D)
	{
		m_moveRight = true;
	}
}

void Camera::onKeyReleased(int key)
{
	if (key == GLFW_KEY_W)
	{
		m_moveForward = false;
	}
	if (key == GLFW_KEY_S)
	{
		m_moveBack = false;
	}
	if (key == GLFW_KEY_A)
	{
		m_moveLeft = false;
	}
	if (key == GLFW_KEY_D)
	{
		m_moveRight = false;
	}
}

void Camera::onMouseOffseted(float dx, float dy)
{
	if (!m_mouseRightButtonPressed)
	{
		return;
	}

	dx *= m_sensitivity;
	dy *= m_sensitivity;

	m_yaw -= dx;
	m_pitch -= dy;
	m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

	updatePose();
}

void Camera::onMousePressed(int button)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		m_mouseRightButtonPressed = true;
	}
}

void Camera::onMouseReleased(int button)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		m_mouseRightButtonPressed = false;
	}
}

void Camera::updatePose()
{
	m_forward.x = std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
	m_forward.y = std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
	m_forward.z = std::sin(glm::radians(m_pitch));
	m_forward = glm::normalize(m_forward);

	m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_up = glm::normalize(glm::cross(m_right, m_forward));
}
