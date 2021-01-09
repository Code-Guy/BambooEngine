#pragma once

#include <map>
#include <functional>

typedef uint32_t InputHandle;

class InputManager
{
public:
	static InputManager& getInstance();
	void init(struct GLFWwindow* window);
	void destroy();

	InputHandle registerKeyPressed(std::function<void(int)> onKeyPressed);
	InputHandle registerKeyReleased(std::function<void(int)> onKeyReleased);
	InputHandle registerMouseMoved(std::function<void(float, float)> onMouseMoved);
	InputHandle registerMouseOffseted(std::function<void(float, float)> onMouseOffseted);
	InputHandle registerMousePressed(std::function<void(int)> onMousePressed);
	InputHandle registerMouseReleased(std::function<void(int)> onMouseReleased);

	void unregisterInputEvent(uint8_t handle);

private:
	static void onKey(struct GLFWwindow* window, int key, int scancode, int action, int mods);
	static void onCursorPos(struct GLFWwindow* window, double xpos, double ypos);
	static void onMouseButton(struct GLFWwindow* window, int button, int action, int mods);

	struct GLFWwindow* m_window;

	std::map<InputHandle, std::function<void(int)>> m_keyPressedMap;
	std::map<InputHandle, std::function<void(int)>> m_keyReleasedMap;
	std::map<InputHandle, std::function<void(float, float)>> m_mouseMovedMap;
	std::map<InputHandle, std::function<void(float, float)>> m_mouseOffsetedMap;
	std::map<InputHandle, std::function<void(int)>> m_mousePressedMap;
	std::map<InputHandle, std::function<void(int)>> m_mouseReleasedMap;

	InputHandle m_inputHandle;

	double m_lastMouseX;
	double m_lastMouseY;
};