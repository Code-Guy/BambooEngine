#pragma once

#include <map>
#include <functional>

class InputManager
{
public:
	static InputManager& getInstance();
	void init(struct GLFWwindow* window);

	uint32_t registerKeyPressed(std::function<void(int)> onKeyPressed);
	uint32_t registerMouseMoved(std::function<void(float, float)> onMouseMoved);
	uint32_t registerMouseOffseted(std::function<void(float, float)> onMouseOffseted);

	void unregisterKeyPressed(uint32_t handle);
	void unregisterMouseMoved(uint32_t handle);
	void unregisterMouseOffseted(uint32_t handle);

private:
	static void onKey(struct GLFWwindow* window, int key, int scancode, int action, int mods);
	static void onCursorPos(struct GLFWwindow* window, double xpos, double ypos);
	static void onMouseButton(struct GLFWwindow* window, int button, int action, int mods);

	struct GLFWwindow* m_window;

	std::map<uint32_t, std::function<void(int)>> m_keyPressedMap;
	std::map<uint32_t, std::function<void(float, float)>> m_mouseMovedMap;
	std::map<uint32_t, std::function<void(float, float)>> m_mouseOffsetedMap;

	uint32_t m_keyPressedHandle;
	uint32_t m_mouseMovedHandle;
	uint32_t m_mouseOffsetedHandle;

	double lastMouseX;
	double lastMouseY;
};