#pragma once

//#include <rttr/rttr_enable.h>

enum struct GamepadButton
{
	BUTTON_SOUTH = 0,
	BUTTON_EAST = 1,
	BUTTON_WEST = 2,
	BUTTON_NORTH = 3,

	SHOULDER_LEFT = 4,
	SHOULDER_RIGHT = 5,

	BUTTON_SELECT = 6,
	BUTTON_START = 7,

	// Button 8 is not used

	STICK_LEFT = 9,
	STICK_RIGHT = 10,

	DPAD_UP = 11,
	DPAD_RIGHT = 12,
	DPAD_DOWN = 13,
	DPAD_LEFT = 14
};

enum struct GamepadAxis
{
	STICK_LEFT_X = 0,
	STICK_LEFT_Y = 1,
	STICK_RIGHT_X = 2,
	STICK_RIGHT_Y = 3,
	TRIGGER_LEFT = 4,
	TRIGGER_RIGHT = 5
};

enum struct MouseButton
{
	MOUSE_BUTTON_LEFT = 0,
	MOUSE_BUTTON_RIGHT = 1,
	MOUSE_BUTTON_MIDDLE = 2
};

// TODO: add more keys
enum struct Key
{
	RIGHT = 262,
	LEFT = 263,
	DOWN = 264,
	UP = 265,
	SPACE = 32,
	ESCAPE = 256,
	ENTER = 257,

	A = 65,
	B = 66,
	C = 67,
	D = 68,
	E = 69,
	F = 70,
	G = 71,
	H = 72,
	I = 73,
	J = 74,
	K = 75,
	L = 76,
	M = 77,
	N = 78,
	O = 79,
	P = 80,
	Q = 81,
	R = 82,
	S = 83,
	T = 84,
	U = 85,
	V = 86,
	W = 87,
	X = 88,
	Y = 89,
	Z = 90
};

enum struct CursorMode
{
	NORMAL,
	HIDDEN,
	DISABLED,
	CAPTURED
};

typedef void (*WindowGLProc)(void);

class Window
{
	//RTTR_ENABLE()

public:
	static Window& Get();

public:
	Window() = default;
	virtual ~Window() = default;

	virtual bool Initialize() = 0;
	virtual void Shutdown() = 0;

	virtual bool IsOpen() = 0;
	virtual void PollEvents() = 0;
	virtual void Display() = 0;

	virtual void GetSize(int* width, int* height) = 0;
	virtual void SetCursorMode(CursorMode mode) = 0;

	virtual float GetAxis(GamepadAxis axis) = 0;
	virtual bool GetButton(GamepadButton button) = 0;
	virtual bool GetButtonOnce(GamepadButton button) = 0;
	virtual bool GetKey(Key key) = 0;
	virtual bool GetKeyOnce(Key key) = 0;
	virtual bool GetMouseButton(MouseButton button) = 0;
	virtual bool GetMouseButtonOnce(MouseButton button) = 0;
	virtual float GetMouseX() = 0;
	virtual float GetMouseY() = 0;
	virtual int GetNumTouches() = 0;
	virtual int GetTouchId(int index) = 0;
	virtual float GetTouchX(int index) = 0;
	virtual float GetTouchY(int index) = 0;
	virtual void SetPadVibration(int leftRumble, int rightRumble) = 0;
	virtual void SetPadLightbarColor(float r, float g, float b) = 0;
	virtual void ResetPadLightbarColor() = 0;

	virtual WindowGLProc GetProcAddress(const char* name) = 0;

#ifdef EDITOR_BUILD
	// ImGui calls (tmp solution until imgui is fully done via interface)
	virtual bool InitializeImGui() = 0;
	virtual void ShutdownImGui() = 0;
	virtual void NewFrameImGui() = 0;
	virtual void EndFrameImGui() = 0;
#endif
};