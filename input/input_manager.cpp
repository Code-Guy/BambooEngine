#include "input_manager.h"
#include <GLFW/glfw3.h>

InputManager& InputManager::getInstance()
{
	static InputManager instance;
	return instance;
}

void InputManager::init(GLFWwindow* window)
{
	m_window = window;
	m_keyPressedHandle = 0;
	m_mouseMovedHandle = 0;
	lastMouseX = 0.0;
	lastMouseY = 0.0;

	glfwSetKeyCallback(window, onKey);
	glfwSetCursorPosCallback(window, onCursorPos);
	glfwSetMouseButtonCallback(window, onMouseButton);
}

uint32_t InputManager::registerKeyPressed(std::function<void(int)> onKeyPressed)
{
	m_keyPressedMap[m_keyPressedHandle] = onKeyPressed;
	return m_keyPressedHandle++;
}

uint32_t InputManager::registerMouseMoved(std::function<void(float, float)> onMouseMoved)
{
	m_mouseMovedMap[m_mouseMovedHandle] = onMouseMoved;
	return m_mouseMovedHandle++;
}

uint32_t InputManager::registerMouseOffseted(std::function<void(float, float)> onMouseOffseted)
{
	m_mouseOffsetedMap[m_mouseOffsetedHandle] = onMouseOffseted;
	return m_mouseOffsetedHandle++;
}

void InputManager::unregisterKeyPressed(uint32_t handle)
{
	if (m_keyPressedMap.find(handle) != m_keyPressedMap.end())
	{
		m_keyPressedMap.erase(handle);
	}
}

void InputManager::unregisterMouseMoved(uint32_t handle)
{
	if (m_mouseMovedMap.find(handle) != m_mouseMovedMap.end())
	{
		m_mouseMovedMap.erase(handle);
	}
}

void InputManager::unregisterMouseOffseted(uint32_t handle)
{
	if (m_mouseOffsetedMap.find(handle) != m_mouseOffsetedMap.end())
	{
		m_mouseOffsetedMap.erase(handle);
	}
}

void InputManager::onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	for (const auto& iter : InputManager::getInstance().m_keyPressedMap)
	{
		iter.second(key);
	}
}

void InputManager::onCursorPos(GLFWwindow* window, double xpos, double ypos)
{
	InputManager& input = InputManager::getInstance();

	for (const auto& iter : input.m_mouseMovedMap)
	{
		iter.second(static_cast<float>(xpos), static_cast<float>(ypos));
	}

	double xoffset = 0.0;
	double yoffset = 0.0;
	if (input.lastMouseX != 0.0)
	{
		xoffset = xpos - input.lastMouseX;
	}
	if (input.lastMouseY != 0.0)
	{
		yoffset = ypos - input.lastMouseY;
	}
	input.lastMouseX = xpos;
	input.lastMouseY = ypos;
	
	for (const auto& iter : input.m_mouseOffsetedMap)
	{
		iter.second(static_cast<float>(xoffset), static_cast<float>(yoffset));
	}
}

void InputManager::onMouseButton(GLFWwindow* window, int button, int action, int mods)
{

}

