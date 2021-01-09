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
	m_inputHandle = 0;
	m_lastMouseX = 0.0;
	m_lastMouseY = 0.0;

	glfwSetKeyCallback(window, onKey);
	glfwSetCursorPosCallback(window, onCursorPos);
	glfwSetMouseButtonCallback(window, onMouseButton);
}

void InputManager::destroy()
{

}

InputHandle InputManager::registerKeyPressed(std::function<void(int)> onKeyPressed)
{
	m_keyPressedMap[m_inputHandle] = onKeyPressed;
	return m_inputHandle++;
}

InputHandle InputManager::registerKeyReleased(std::function<void(int)> onKeyReleased)
{
	m_keyReleasedMap[m_inputHandle] = onKeyReleased;
	return m_inputHandle++;
}

InputHandle InputManager::registerMouseMoved(std::function<void(float, float)> onMouseMoved)
{
	m_mouseMovedMap[m_inputHandle] = onMouseMoved;
	return m_inputHandle++;
}

InputHandle InputManager::registerMouseOffseted(std::function<void(float, float)> onMouseOffseted)
{
	m_mouseOffsetedMap[m_inputHandle] = onMouseOffseted;
	return m_inputHandle++;
}

InputHandle InputManager::registerMousePressed(std::function<void(int)> onMousePressed)
{
	m_mousePressedMap[m_inputHandle] = onMousePressed;
	return m_inputHandle++;
}

InputHandle InputManager::registerMouseReleased(std::function<void(int)> onMouseReleased)
{
	m_mouseReleasedMap[m_inputHandle] = onMouseReleased;
	return m_inputHandle++;
}

void InputManager::unregisterInputEvent(uint8_t handle)
{
	if (m_keyPressedMap.find(handle) != m_keyPressedMap.end())
	{
		m_keyPressedMap.erase(handle);
		return;
	}

	if (m_keyReleasedMap.find(handle) != m_keyReleasedMap.end())
	{
		m_keyReleasedMap.erase(handle);
		return;
	}

	if (m_mouseMovedMap.find(handle) != m_mouseMovedMap.end())
	{
		m_mouseMovedMap.erase(handle);
		return;
	}

	if (m_mouseOffsetedMap.find(handle) != m_mouseOffsetedMap.end())
	{
		m_mouseOffsetedMap.erase(handle);
		return;
	}

	if (m_mousePressedMap.find(handle) != m_mousePressedMap.end())
	{
		m_mouseOffsetedMap.erase(handle);
		return;
	}

	if (m_mouseReleasedMap.find(handle) != m_mouseReleasedMap.end())
	{
		m_mouseOffsetedMap.erase(handle);
		return;
	}
}

void InputManager::onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		for (const auto& iter : InputManager::getInstance().m_keyPressedMap)
		{
			iter.second(key);
		}
	}
	else if (action == GLFW_RELEASE)
	{
		for (const auto& iter : InputManager::getInstance().m_keyReleasedMap)
		{
			iter.second(key);
		}
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
	if (input.m_lastMouseX != 0.0)
	{
		xoffset = xpos - input.m_lastMouseX;
	}
	if (input.m_lastMouseY != 0.0)
	{
		yoffset = ypos - input.m_lastMouseY;
	}
	input.m_lastMouseX = xpos;
	input.m_lastMouseY = ypos;
	
	for (const auto& iter : input.m_mouseOffsetedMap)
	{
		iter.second(static_cast<float>(xoffset), static_cast<float>(yoffset));
	}
}

void InputManager::onMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		for (const auto& iter : InputManager::getInstance().m_mousePressedMap)
		{
			iter.second(button);
		}
	}
	else if (action == GLFW_RELEASE)
	{
		for (const auto& iter : InputManager::getInstance().m_mouseReleasedMap)
		{
			iter.second(button);
		}
	}
}

