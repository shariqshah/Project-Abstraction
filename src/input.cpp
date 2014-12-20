#include <vector>
#include <algorithm>
#include "scriptengine.h"
#include "input.h"

namespace Input
{
	namespace
	{
        std::vector<int>releasedKeys;
		const int cNumKeyboardKeys = 283;
		const int cNumMouseButtons = 5;
		const int cMaxDelta = 50;
        Uint8  sKeyState[cNumKeyboardKeys];
        Uint8  sMouseButtonState[cNumMouseButtons];
		Sint32 sMouseX, sMouseY;
		Sint32 sMouseRelX, sMouseRelY;
	}
	
	void updateKeys(SDL_KeyboardEvent event)
	{
		sKeyState[event.keysym.scancode] = event.state;
		
		if(event.state == (Uint8)KeyState::RELEASED)
			releasedKeys.push_back(event.keysym.scancode);
	}

	void updateMouseButtons(SDL_MouseButtonEvent event)
	{
		sMouseButtonState[event.button] = event.state;
	}

	bool isPressed(Key key)
	{
		if(sKeyState[SDL_GetScancodeFromKey((SDL_Keycode)key)] == (Uint8)KeyState::PRESSED)
			return true;
		else
			return false;
	}

	bool isReleased(Key key)
	{
		if(sKeyState[SDL_GetScancodeFromKey((SDL_Keycode)key)] == (Uint8)KeyState::RELEASED)
			return true;
		else
			return false;
	}

	void showCursor(bool show)
	{
		if(show)
			SDL_ShowCursor(1);
		else
			SDL_ShowCursor(0);
	}

	void setCursorLock(bool lock)
	{
		if(lock)
			SDL_SetRelativeMouseMode(SDL_TRUE);
		else
			SDL_SetRelativeMouseMode(SDL_FALSE);
	}

	bool isCursorLocked()
	{
		SDL_bool status = SDL_GetRelativeMouseMode();

		if(status == SDL_TRUE)
			return true;
		else
			return false;
	}

	bool isCursorVisible()
	{
		int status = SDL_ShowCursor(-1);

		if(status == 1)
			return true;
		else if(status == 0)
			return false;
		else
			return false;
	}

	bool isPressed(MouseButton button)
	{
		if(sMouseButtonState[(int)button] == SDL_PRESSED)
			return true;
		else
			return false;
	}

	bool isReleased(MouseButton button)
	{
		if(sMouseButtonState[(int)button] == SDL_RELEASED)
			return true;
		else
			return false;
	}

	void updateReleasedKeys()
	{
		for(int key : releasedKeys)
			sKeyState[key] = (Uint8)KeyState::INACTIVE;

		for(int iCount = 0; iCount < cNumMouseButtons; iCount++)
		{
			if(sMouseButtonState[iCount] == (Uint8)KeyState::RELEASED)
				sMouseButtonState[iCount] = (Uint8)KeyState::INACTIVE;
		}

		releasedKeys.clear();
	}

	void initializeInput()
	{
		SDL_PumpEvents();

		// Initialize Keyboard and mouse keys to inactive
		for(int iCount = 0; iCount < cNumKeyboardKeys; iCount++)
			sKeyState[iCount] = (Uint8)KeyState::INACTIVE;

		for(int iCount = 0; iCount < cNumMouseButtons; iCount++)
			sMouseButtonState[iCount] = (Uint8)KeyState::INACTIVE;

		// Get cursor positon and relative cursor position
		SDL_GetMouseState(&sMouseX, &sMouseY);
		SDL_GetRelativeMouseState(&sMouseRelX, &sMouseRelY);
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}

	void updateMousePostion(SDL_MouseMotionEvent event)
	{
		sMouseX = event.x;
		sMouseY = event.y;

		sMouseRelX = event.xrel;
		sMouseRelY = event.yrel;

		// if delta is unrealisticly large or small for some reason, ignore it
		if(sMouseRelX > cMaxDelta || sMouseRelX < -cMaxDelta)
			sMouseRelX = 0;
		if(sMouseRelY > cMaxDelta || sMouseRelY < -cMaxDelta)
			sMouseRelY = 0;
	}

	Sint32 getMouseX()
	{
		return sMouseX;
	}
	
	Sint32 getMouseY()
	{
		return sMouseY;
	}
	
	Sint32 getMouseRelX()
	{
		return sMouseRelX;
	}
	
	Sint32 getMouseRelY()
	{
		return sMouseRelY;
	}

	void resetMouseRel()
	{
		sMouseRelX = sMouseRelY = 0;
	}

	bool isKeyPressed(Key key)
	{
		return isPressed(key);
	}

	bool isKeyReleased(Key key)
	{
		return isReleased(key);
	}

	bool isMousePressed(MouseButton button)
	{
		return isPressed(button);
	}

	bool isMouseReleased(MouseButton button)
	{
		return isReleased(button);
	}

	void generateBindings()
	{
		Sqrat::ConstTable().Enum("Key", Sqrat::Enumeration()
								 .Const("A", Key::A)
								 .Const("B", Key::B)
								 .Const("C", Key::C)
								 .Const("D", Key::D)
								 .Const("E", Key::E)
								 .Const("F", Key::F)
								 .Const("G", Key::G)
								 .Const("H", Key::H)
								 .Const("I", Key::I)
								 .Const("J", Key::J)
								 .Const("K", Key::K)
								 .Const("L", Key::L)
								 .Const("M", Key::M)
								 .Const("N", Key::N)
								 .Const("O", Key::O)
								 .Const("P", Key::P)
								 .Const("Q", Key::Q)
								 .Const("R", Key::R)
								 .Const("S", Key::S)
								 .Const("T", Key::T)
								 .Const("U", Key::U)
								 .Const("V", Key::V)
								 .Const("W", Key::W)
								 .Const("X", Key::X)
								 .Const("Y", Key::Y)
								 .Const("Z", Key::Z)
								 .Const("K0", Key::K0)
								 .Const("K1", Key::K1)
								 .Const("K2", Key::K2)
								 .Const("K3", Key::K3)
								 .Const("K4", Key::K4)
								 .Const("K5", Key::K5)
								 .Const("K6", Key::K6)
								 .Const("K7", Key::K7)
								 .Const("K8", Key::K8)
								 .Const("K9", Key::K9)
								 .Const("F1", Key::F1)
								 .Const("F2", Key::F2)
								 .Const("F3", Key::F3)
								 .Const("F4", Key::F4)
								 .Const("F5", Key::F5)
								 .Const("F6", Key::F6)
								 .Const("F7", Key::F7)
								 .Const("F8", Key::F8)
								 .Const("F9", Key::F9)
								 .Const("F10", Key::F10)
								 .Const("F11", Key::F11)
								 .Const("F12", Key::F12)
								 .Const("UP", Key::UP)
								 .Const("DOWN", Key::DOWN)
								 .Const("LEFT", Key::LEFT)
								 .Const("RIGHT", Key::RIGHT)
								 .Const("LSHIFT", Key::LSHIFT)
								 .Const("RSHIFT", Key::RSHIFT)
								 .Const("LCTRL", Key::LCTRL)
								 .Const("RCTRL", Key::RCTRL)
								 .Const("SPACE", Key::SPACE)
								 .Const("ENTER", Key::ENTER)
								 .Const("NP_PLUS", Key::NP_PLUS)
								 .Const("NP_MINUS", Key::NP_MINUS));

		Sqrat::ConstTable().Enum("MouseButton", Sqrat::Enumeration()
								 .Const("M_LEFT",   MouseButton::M_LEFT)
								 .Const("M_RIGHT",  MouseButton::M_RIGHT)
								 .Const("M_MIDDLE", MouseButton::M_MIDDLE)
								 .Const("M_X1",     MouseButton::M_X1)
								 .Const("M_X2",     MouseButton::M_X2));

		Sqrat::RootTable().Bind("Input", Sqrat::Table(ScriptEngine::getVM())
								.Func("getMouseX",       &getMouseX)
								.Func("getMouseY",       &getMouseY)
								.Func("getMouseRelX",    &getMouseRelX)
								.Func("getMouseRelY",    &getMouseRelY)
								.Func("isKeyPressed",    &isKeyPressed)
								.Func("isKeyReleased",   &isKeyReleased)
								.Func("isMousePressed",  &isMousePressed)
								.Func("isMouseReleased", &isMouseReleased)
								.Func("showCursor",      &showCursor)
								.Func("lockCursor",      &setCursorLock)
								.Func("isCursorVisible", &isCursorVisible)
								.Func("isCursorLocked",  &isCursorLocked));
	}
}
