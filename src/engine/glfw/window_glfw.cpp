#ifdef WINDOW_GLFW_BACKEND
#include <engine/window.hpp>
#include <engine/log.hpp>
#include <engine/enum.hpp>
#include <engine/macros.hpp>

#include <stdlib.h>

//#include <rttr/registration.h>
//RTTR_PLUGIN_REGISTRATION
//{
//	rttr::registration::class_<WindowGLFW>("WindowGLFW")
//		.constructor();
////.method("GetWindowPtr", &WindowGLFW::GetWindowPtr);
//}

#include <GLFW/glfw3.h>

class WindowGLFW final : public Window
{
	//RTTR_ENABLE(Window)

public:
	static WindowGLFW& Get();

private:
	WindowGLFW() = default;
	~WindowGLFW() = default;

	WindowGLFW(const WindowGLFW&) = delete;
	WindowGLFW& operator=(const WindowGLFW&) = delete;

	WindowGLFW(WindowGLFW&&) = delete;
	WindowGLFW& operator=(WindowGLFW&&) = delete;

public:
	bool Initialize() override;
	void Shutdown() override;

	bool IsOpen() override;
	void PollEvents() override;
	void Display() override;

	void GetSize(int* width, int* height) override;
	void SetCursorMode(CursorMode mode) override;

	float GetAxis(GamepadAxis axis) override;
	bool GetButton(GamepadButton button) override;
	bool GetButtonOnce(GamepadButton button) override;
	bool GetKey(Key key) override;
	bool GetKeyOnce(Key key) override;
	bool GetMouse() override;
	bool GetMouseButton(MouseButton button) override;
	bool GetMouseButtonOnce(MouseButton button) override;
	float GetMouseX() override;
	float GetMouseY() override;
	int GetNumTouches() override;
	int GetTouchId(int index) override;
	float GetTouchX(int index) override;
	float GetTouchY(int index) override;
	void SetPadVibration(int leftRumble, int rightRumble) override;
	void SetPadLightbarColor(float r, float g, float b) override;
	void ResetPadLightbarColor() override;

	WindowGLProc GetProcAddress(const char* name) override;

	bool InitializeImGui() override { return true; }
	void ShutdownImGui() override {}
	void NewFrameImGui() override {}
	void EndFrameImGui() override {}

public:
	GLFWwindow* GetWindowPtr() const;

private:
	static void glfw_error_callback(int i, const char* c);
	static void glfw_window_size_callback(GLFWwindow* window, int width, int height);
	static void glfw_joystick_callback(int joy, int event);
	static void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void glfw_mousebutton_callback(GLFWwindow* window, int button, int action, int mods);

private:
	GLFWwindow* m_pWindow = nullptr;

	GLFWgamepadstate m_currGamepadState{};
	GLFWgamepadstate m_prevGamepadState{};

	static constexpr int nr_keys = 350;
	bool m_currKeysDown[nr_keys]{};
	bool m_prevKeysDown[nr_keys]{};
	bool m_keysDownChanged[nr_keys]{};

	static constexpr int nr_mousebuttons = 8;
	bool m_currMouseButtonsDown[nr_mousebuttons]{};
	bool m_prevMouseButtonsDown[nr_mousebuttons]{};
	bool m_mouseButtonsDownChanged[nr_keys]{};

	float m_mousePos[2]{};
	bool m_gamepadConnected = false;
};

Window& Window::Get()
{
	return WindowGLFW::Get();
}

WindowGLFW& WindowGLFW::Get()
{
	static WindowGLFW instance;
	return instance;
}

bool WindowGLFW::IsOpen()
{
	return !glfwWindowShouldClose(m_pWindow);
}

void WindowGLFW::GetSize(int* width, int* height)
{
	glfwGetFramebufferSize(m_pWindow, width, height);
}

void WindowGLFW::SetCursorMode(CursorMode mode)
{
	switch (mode)
	{
	case CursorMode::NORMAL:
		glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;
	case CursorMode::HIDDEN:
		glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		break;
	case CursorMode::DISABLED:
		glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		break;
	case CursorMode::CAPTURED:
		glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
		break;
	default:
		glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

WindowGLProc WindowGLFW::GetProcAddress(const char* name)
{
	return glfwGetProcAddress(name);
}

void WindowGLFW::glfw_error_callback(int i, const char* c)
{
	LOGE(Window, "GLFW ({}) {}", i, c);
}

void WindowGLFW::glfw_window_size_callback(GLFWwindow* window, int width, int height)
{
}

void WindowGLFW::glfw_joystick_callback(int joy, int event) {}

void WindowGLFW::glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	auto& ctx = static_cast<WindowGLFW&>(Window::Get());

	// get the screen-to-game scaling parameters
	//const auto& screen_to_game = xs::configuration::get_scale_to_game(xs::device::get_width(), xs::device::get_height());

	// translate the mouse position to game coordinates
	//xs::configuration::scale_to_game(static_cast<int>(xpos), static_cast<int>(ypos), screen_to_game, m_mousePos[0], m_mousePos[1]);

	ctx.m_mousePos[0] = (float)xpos;
	ctx.m_mousePos[1] = (float)ypos;
	}

void WindowGLFW::glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto& ctx = static_cast<WindowGLFW&>(Window::Get());
	if (action == GLFW_PRESS || action == GLFW_RELEASE)
		ctx.m_keysDownChanged[key] = true;
}

void WindowGLFW::glfw_mousebutton_callback(GLFWwindow* window, int button, int action, int mods)
{
	auto& ctx = static_cast<WindowGLFW&>(Window::Get());
	if (action == GLFW_PRESS || action == GLFW_RELEASE)
		ctx.m_mouseButtonsDownChanged[button] = true;
}

bool WindowGLFW::Initialize()
{
#ifdef __arm__
	if (putenv((char*)"DISPLAY=:0"))
	{
		LOGE(Window, "Failed to set DISPLAY enviroment variable!");
		return false;
	}
#endif

	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
	{
		LOGE(Window, "Failed to initialize GLFW!");
		return false;
	}

	const String& title = "Titlte";// Data::GetString("Title", "Title", DataTarget::SYSTEM);
	int width = 800;// Data::GetInt("Width", 800, DataTarget::SYSTEM);
	int height = 600;// Data::GetInt("Height", 600, DataTarget::SYSTEM);

	GLFWmonitor* pMonitor = nullptr;

#ifdef GRAPHICS_OPENGL_BACKEND

#if defined DEBUG_BUILD || defined EDITOR_BUILD
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
#endif

#ifdef WIN32
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);

	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

#elif defined __arm__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES, 2);

	pMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);
	width = pMode->width;
	height = pMode->height;

	glfwWindowHint(GLFW_RED_BITS, pMode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, pMode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, pMode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, pMode->refreshRate);
#endif

#else
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
#endif

	m_pWindow = glfwCreateWindow(width, height, title.c_str(), pMonitor, NULL);

	if (m_pWindow == NULL)
	{
		LOGE(Window, "Failed to create GLFW window!");
		glfwTerminate();
		return false;
	}

	glfwGetWindowSize(m_pWindow, &width, &height);
	//Screen::SetWidth(width);
	//Screen::SetHeight(height);

	glfwSetWindowSizeCallback(m_pWindow, glfw_window_size_callback);

#ifdef GRAPHICS_OPENGL_BACKEND
	glfwMakeContextCurrent(m_pWindow);
	glfwSwapInterval(1);

#elif defined GRAPHICS_VULKAN_BACKEND
	if (!glfwVulkanSupported())
	{
		LOGE(Window, "GLFW: Vulkan Not Supported!");
		return false;
	}
#endif

	glfwSetJoystickCallback(glfw_joystick_callback);
	glfwSetCursorPosCallback(m_pWindow, glfw_cursor_position_callback);
	glfwSetKeyCallback(m_pWindow, glfw_key_callback);
	glfwSetMouseButtonCallback(m_pWindow, glfw_mousebutton_callback);

	return true;
}

void WindowGLFW::Shutdown()
{
	glfwSetJoystickCallback(NULL);
	glfwSetCursorPosCallback(m_pWindow, NULL);

	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
}

void WindowGLFW::PollEvents()
{
	//PROFILE_FUNCTION();
	glfwPollEvents();

	if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(m_pWindow, true);

	for (int i = 0; i < nr_keys; ++i)
	{
		m_prevKeysDown[i] = m_currKeysDown[i];
		if (m_keysDownChanged[i])
		{
			m_currKeysDown[i] = !m_currKeysDown[i];
			m_keysDownChanged[i] = false;
		}
	}

	for (int i = 0; i < nr_mousebuttons; ++i)
	{
		m_prevMouseButtonsDown[i] = m_currMouseButtonsDown[i];
		if (m_mouseButtonsDownChanged[i])
		{
			m_currMouseButtonsDown[i] = !m_currMouseButtonsDown[i];
			m_mouseButtonsDownChanged[i] = false;
		}
	}

	m_prevGamepadState = m_currGamepadState;

	if (glfwJoystickPresent(0) && glfwJoystickIsGamepad(0))
		m_gamepadConnected = glfwGetGamepadState(0, &m_currGamepadState);
}

void WindowGLFW::Display()
{
	//PROFILE_FUNCTION();

#ifdef GRAPHICS_OPENGL_BACKEND
	glfwSwapBuffers(m_pWindow);
#endif
}

float WindowGLFW::GetAxis(GamepadAxis axis)
{
	if (!m_gamepadConnected) return 0.0;

	int axis_id = int(axis);
	ENSURE(axis_id >= 0 && axis_id <= GLFW_GAMEPAD_AXIS_LAST);
	return static_cast<float>(m_currGamepadState.axes[axis_id]);
}

bool WindowGLFW::GetButton(GamepadButton button)
{
	if (!m_gamepadConnected) return false;

	int button_id = int(button);
	ENSURE(button_id >= 0 && button_id <= GLFW_GAMEPAD_BUTTON_LAST);
	return static_cast<bool>(m_currGamepadState.buttons[button_id]);
}

bool WindowGLFW::GetButtonOnce(GamepadButton button)
{
	if (!m_gamepadConnected) return false;

	int button_id = int(button);
	ENSURE(button_id >= 0 && button_id <= GLFW_GAMEPAD_BUTTON_LAST);
	return
		!static_cast<bool>(m_prevGamepadState.buttons[button_id]) &&
		static_cast<bool>(m_currGamepadState.buttons[button_id]);
}

bool WindowGLFW::GetKey(Key key)
{
	ENSURE(key >= GLFW_KEY_SPACE && key <= GLFW_KEY_LAST);
	// TODO: CD: In the future with our custom containers we can have an overload that indexes with enum values so the conversion isn't needed
	return m_currKeysDown[Enum::as_value(key)];
}

bool WindowGLFW::GetKeyOnce(Key key)
{
	//BX_ENSURE(key >= GLFW_KEY_SPACE && key <= GLFW_KEY_LAST);
	// TODO: CD: In the future with our custom containers we can have an overload that indexes with enum values so the conversion isn't needed
	return m_currKeysDown[Enum::as_value(key)] && !m_prevKeysDown[Enum::as_value(key)];
}

bool WindowGLFW::GetMouse()
{
	return true;
}

bool WindowGLFW::GetMouseButton(MouseButton button)
{
	return m_currMouseButtonsDown[int(button)];
}

bool WindowGLFW::GetMouseButtonOnce(MouseButton button)
{
	return m_currMouseButtonsDown[int(button)] && !m_prevMouseButtonsDown[int(button)];
}

float WindowGLFW::GetMouseX()
{
	return static_cast<float>(m_mousePos[0]);
}

float WindowGLFW::GetMouseY()
{
	return static_cast<float>(m_mousePos[1]);
}

int WindowGLFW::GetNumTouches()
{
	return 0;
}

int WindowGLFW::GetTouchId(int index)
{
	return 0;
}

float WindowGLFW::GetTouchX(int index)
{
	return 0;
}

float WindowGLFW::GetTouchY(int index)
{
	return 0;
}

void WindowGLFW::SetPadVibration(int leftRumble, int rightRumble)
{
	// Unimplemented on the PC
}

void WindowGLFW::SetPadLightbarColor(float r, float g, float b)
{
	// Unimplemented on the PC (specific dualshock 5 controller mechanic)
}

void WindowGLFW::ResetPadLightbarColor()
{
	// Unimplemented on the PC (specific dualshock 5 controller mechanic)
}

GLFWwindow* WindowGLFW::GetWindowPtr() const
{
	return m_pWindow;
}

#endif // WINDOW_GLFW_BACKEND