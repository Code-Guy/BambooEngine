#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera(glm::vec3 position, 
		float yaw, float pitch,
		float speed, float sensitivity);

	void setFovy(float fovy);
	void setAspect(float aspect);
	void setClipping(float zNear, float zFar);

	void tick(float deltaTime);

	glm::mat4 getViewMatrix();
	glm::mat4 getPerspectiveMatrix();
	glm::mat4 getViewPerspectiveMatrix();

	void onKeyPressed(int key);
	void onKeyReleased(int key);
	void onMouseOffseted(float dx, float dy);
	void onMousePressed(int button);
	void onMouseReleased(int button);

private:
	void updatePose();

	glm::vec3 m_position;
	float m_yaw;
	float m_pitch;

	float m_speed;
	float m_sensitivity;

	float m_fovy;
	float m_aspect;
	float m_zNear;
	float m_zFar;

	glm::vec3 m_forward;
	glm::vec3 m_right;
	glm::vec3 m_up;

	bool m_moveForward;
	bool m_moveBack;
	bool m_moveLeft;
	bool m_moveRight;

	bool m_mouseRightButtonPressed;
};