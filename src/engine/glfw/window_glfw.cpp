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

#ifdef EDITOR_BUILD
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#endif

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

	void GetSize(i32* width, i32* height) override;
	void GetContentScale(f32* xscale, f32* yscale) override;

	void SetCursorMode(CursorMode mode) override;

	f32 GetAxis(GamepadAxis axis) override;
	bool GetButton(GamepadButton button) override;
	bool GetButtonOnce(GamepadButton button) override;
	bool GetKey(Key key) override;
	bool GetKeyOnce(Key key) override;
	bool GetMouseButton(MouseButton button) override;
	bool GetMouseButtonOnce(MouseButton button) override;
	f32 GetMouseX() override;
	f32 GetMouseY() override;
	i32 GetNumTouches() override;
	i32 GetTouchId(i32 index) override;
	f32 GetTouchX(i32 index) override;
	f32 GetTouchY(i32 index) override;
	void SetPadVibration(i32 leftRumble, i32 rightRumble) override;
	void SetPadLightbarColor(f32 r, f32 g, f32 b) override;
	void ResetPadLightbarColor() override;

	WindowGLProc GetProcAddress(const char* name) override;

#ifdef EDITOR_BUILD
	bool InitializeImGui() override
	{
#if defined(GRAPHICS_OPENGL_BACKEND) || defined(GRAPHICS_OPENGLES_BACKEND)
		if (!ImGui_ImplGlfw_InitForOpenGL(m_pWindow, true))
#endif
		{
			LOGE(Window, "Failed to initialize ImGui GLFW backend!");
			return false;
		}
		return true;
	}

	void ShutdownImGui() override
	{
		ImGui_ImplGlfw_Shutdown();
	}

	void NewFrameImGui() override
	{
		ImGui_ImplGlfw_NewFrame();
	}

	void EndFrameImGui() override
	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}
#endif

public:
	GLFWwindow* GetWindowPtr() const;

private:
	static void glfw_error_callback(i32 i, const char* c);
	static void glfw_window_size_callback(GLFWwindow* window, i32 width, i32 height);
	static void glfw_joystick_callback(i32 joy, i32 event);
	static void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void glfw_key_callback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods);
	static void glfw_mousebutton_callback(GLFWwindow* window, i32 button, i32 action, i32 mods);

private:
	GLFWwindow* m_pWindow = nullptr;
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

void WindowGLFW::GetSize(i32* width, i32* height)
{
	glfwGetFramebufferSize(m_pWindow, width, height);
}


void WindowGLFW::GetContentScale(f32* xscale, f32* yscale)
{
	glfwGetWindowContentScale(m_pWindow, xscale, yscale);
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

void WindowGLFW::glfw_error_callback(i32 i, const char* c)
{
	LOGE(Window, "GLFW ({}) {}", i, c);
}

void WindowGLFW::glfw_window_size_callback(GLFWwindow* window, i32 width, i32 height)
{
	auto& ctx = static_cast<WindowGLFW&>(Window::Get());
}

void WindowGLFW::glfw_joystick_callback(i32 joy, i32 event)
{
	auto& ctx = static_cast<WindowGLFW&>(Window::Get());
}

void WindowGLFW::glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	auto& ctx = static_cast<WindowGLFW&>(Window::Get());
}

void WindowGLFW::glfw_key_callback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods)
{
	auto& ctx = static_cast<WindowGLFW&>(Window::Get());
}

void WindowGLFW::glfw_mousebutton_callback(GLFWwindow* window, i32 button, i32 action, i32 mods)
{
	auto& ctx = static_cast<WindowGLFW&>(Window::Get());
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
	i32 width = 800;// Data::GetInt("Width", 800, DataTarget::SYSTEM);
	i32 height = 600;// Data::GetInt("Height", 600, DataTarget::SYSTEM);

	GLFWmonitor* pMonitor = nullptr;

#if defined(GRAPHICS_OPENGL_BACKEND) || defined(GRAPHICS_OPENGLES_BACKEND)

#if defined DEBUG_BUILD || defined EDITOR_BUILD
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
#endif

#ifdef GRAPHICS_OPENGL_BACKEND
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);

	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

#elif defined GRAPHICS_OPENGLES_BACKEND
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

#if defined(GRAPHICS_OPENGL_BACKEND) || defined(GRAPHICS_OPENGLES_BACKEND)
	glfwMakeContextCurrent(m_pWindow);
	glfwSwapInterval(1);

#elif defined(GRAPHICS_VULKAN_BACKEND)
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
	glfwSetKeyCallback(m_pWindow, NULL);
	glfwSetMouseButtonCallback(m_pWindow, NULL);

	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
}

void WindowGLFW::PollEvents()
{
	// Poll GLFW events
	glfwPollEvents();

	// Handle the ESC key to close the window
	if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(m_pWindow, true);
}

void WindowGLFW::Display()
{
	//PROFILE_FUNCTION();

#if defined(GRAPHICS_OPENGL_BACKEND) || defined(GRAPHICS_OPENGLES_BACKEND)
	glfwSwapBuffers(m_pWindow);
#endif
}

f32 WindowGLFW::GetAxis(GamepadAxis axis)
{
	FAIL("TODO: Implement");
	return 0;
}

bool WindowGLFW::GetButton(GamepadButton button)
{
	FAIL("TODO: Implement");
	return false;
}

bool WindowGLFW::GetButtonOnce(GamepadButton button)
{
	FAIL("TODO: Implement");
	return false;
}

bool WindowGLFW::GetKey(Key key)
{
	ENSURE(key >= GLFW_KEY_SPACE && key <= GLFW_KEY_LAST);
	return glfwGetKey(m_pWindow, Enum::as_value(key)) == GLFW_PRESS;
}

bool WindowGLFW::GetKeyOnce(Key key)
{
	FAIL("TODO: Implement");
	return false;
}

bool WindowGLFW::GetMouseButton(MouseButton button)
{
	return glfwGetMouseButton(m_pWindow, Enum::as_value(button)) == GLFW_PRESS;
}

bool WindowGLFW::GetMouseButtonOnce(MouseButton button)
{
	FAIL("TODO: Implement");
	return false;
}

f32 WindowGLFW::GetMouseX()
{
	f64 x, y;
	glfwGetCursorPos(m_pWindow, &x, &y);
	return (f32)x;
}

f32 WindowGLFW::GetMouseY()
{
	f64 x, y;
	glfwGetCursorPos(m_pWindow, &x, &y);
	return (f32)y;
}

i32 WindowGLFW::GetNumTouches()
{
	FAIL("TODO: Implement");
	return 0;
}

i32 WindowGLFW::GetTouchId(i32 index)
{
	FAIL("TODO: Implement");
	return 0;
}

f32 WindowGLFW::GetTouchX(i32 index)
{
	FAIL("TODO: Implement");
	return 0;
}

f32 WindowGLFW::GetTouchY(i32 index)
{
	FAIL("TODO: Implement");
	return 0;
}

void WindowGLFW::SetPadVibration(i32 leftRumble, i32 rightRumble)
{
	// Unimplemented on the PC?
	FAIL("TODO: Implement");
}

void WindowGLFW::SetPadLightbarColor(f32 r, f32 g, f32 b)
{
	// Unimplemented on the PC? (specific dualshock 5 controller mechanic)
	FAIL("TODO: Implement");
}

void WindowGLFW::ResetPadLightbarColor()
{
	// Unimplemented on the PC? (specific dualshock 5 controller mechanic)
	FAIL("TODO: Implement");
}

GLFWwindow* WindowGLFW::GetWindowPtr() const
{
	return m_pWindow;
}