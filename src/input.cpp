#include <vector>
#include <algorithm>
#include "scriptengine.h"
#include "input.h"
#include "gui.h"
namespace Input
{
	namespace
	{
        std::vector<int>releasedKeys;
		const int NUM_KEYBOARD_KEYS = 283;
		const int NUM_MOUSE_BUTTONS = 5;
		const int MAX_DELTA = 50;
        Uint8  keyState[NUM_KEYBOARD_KEYS];
        Uint8  mouseButtonState[NUM_MOUSE_BUTTONS];
		Sint32 mouseX, mouseY;
		Sint32 mouseRelX, mouseRelY;
		Sint32 scrollX, scrollY;
		bool   textInputEnabled = false;
	}
	
	void updateKeys(SDL_KeyboardEvent event)
	{
		bool  isDown  = true;
		Uint8 key     = event.keysym.scancode;
		keyState[key] = event.state;		
		if(event.state == (Uint8)KeyState::RELEASED)
		{
			releasedKeys.push_back(key);
			isDown = false;
		}
		
		SDL_Keymod modState = SDL_GetModState();
		bool modCtrl  = (modState & KMOD_CTRL);
		bool modShift = (modState & KMOD_SHIFT);
		
		Gui::updateKeyDown(key, isDown, modCtrl, modShift);
	}

	void updateMouseButtons(SDL_MouseButtonEvent event)
	{
		mouseButtonState[event.button] = event.state;
	}

	bool isPressed(Key key)
	{
		bool pressed = false;
		if(!textInputEnabled)
		{
			if(keyState[SDL_GetScancodeFromKey((SDL_Keycode)key)] == (Uint8)KeyState::PRESSED)
				pressed = true;
		}
		return pressed;
	}

	bool isReleased(Key key)
	{
		bool released = false;
		if(!textInputEnabled)
		{
			if(keyState[SDL_GetScancodeFromKey((SDL_Keycode)key)] == (Uint8)KeyState::RELEASED)
				released = true;
		}
		return released;
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
		if(mouseButtonState[(int)button] == SDL_PRESSED)
			return true;
		else
			return false;
	}

	bool isReleased(MouseButton button)
	{
		if(mouseButtonState[(int)button] == SDL_RELEASED)
			return true;
		else
			return false;
	}

    void updateReleasedKeys()
	{
		for(int key : releasedKeys)
			keyState[key] = (Uint8)KeyState::INACTIVE;

        for(int count = 0; count < NUM_MOUSE_BUTTONS; count++)
		{
            if(mouseButtonState[count] == (Uint8)KeyState::RELEASED)
                mouseButtonState[count] = (Uint8)KeyState::INACTIVE;
		}

		releasedKeys.clear();
	}

	void initializeInput()
	{
		SDL_PumpEvents();

		// Initialize Keyboard and mouse keys to inactive
		for(int count = 0; count < NUM_KEYBOARD_KEYS; count++)
		{
			keyState[count] = (Uint8)KeyState::INACTIVE;
			Gui::updateKeyDown(SDL_GetScancodeFromKey(count), false, false, false);
		}

		for(int count = 0; count < NUM_MOUSE_BUTTONS; count++)
			mouseButtonState[count] = (Uint8)KeyState::INACTIVE;

		// Get cursor positon and relative cursor position
		SDL_GetMouseState(&mouseX, &mouseY);
		SDL_GetRelativeMouseState(&mouseRelX, &mouseRelY);
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}

	void updateMousePostion(SDL_MouseMotionEvent event)
	{
		mouseX = event.x;
		mouseY = event.y;

		mouseRelX = event.xrel;
		mouseRelY = event.yrel;

		// if delta is unrealisticly large or small for some reason, ignore it
		if(mouseRelX > MAX_DELTA || mouseRelX < -MAX_DELTA)
			mouseRelX = 0;
		if(mouseRelY > MAX_DELTA || mouseRelY < -MAX_DELTA)
			mouseRelY = 0;
	}

	void updateScroll(SDL_MouseWheelEvent event)
	{
		scrollX = event.x;
		scrollY = event.y;

		Gui::updateMouseWheel(scrollY);
	}

	Sint32 getMouseX()
	{
		return mouseX;
	}
	
	Sint32 getMouseY()
	{
		return mouseY;
	}
	
	Sint32 getMouseRelX()
	{
		return mouseRelX;
	}
	
	Sint32 getMouseRelY()
	{
		return mouseRelY;
	}

	Sint32 getScrollX()
	{
		return scrollX;
	}

	Sint32 getScrollY()
	{
		return scrollY;
	}

	void resetMouseRel()
	{
		mouseRelX = mouseRelY = 0;
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
								 .Const("RSUPER", Key::RSUPER)
								 .Const("LSUPER", Key::LSUPER)
								 .Const("SPACE", Key::SPACE)
								 .Const("ENTER", Key::ENTER)
								 .Const("ESC", Key::ESC)
								 .Const("TAB", Key::TAB)
								 .Const("HOME", Key::HOME)
								 .Const("END", Key::END)
								 .Const("DELETE", Key::DELETE)
								 .Const("BACKSPC", Key::BACKSPC)
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
	
    void enableTextInput(bool enable)
	{
        textInputEnabled = enable;

		if(textInputEnabled)
			SDL_StartTextInput();
		else
		{
			SDL_StopTextInput();
			Uint8 key = SDL_GetScancodeFromKey(Key::ESC);
            keyState[key] = (Uint8)KeyState::INACTIVE;
		}
	}

	void textEntered(const char* text)
	{
		Gui::textEntered(text);
	}

	bool isTextInputActive()
	{
		return textInputEnabled;
	}
	
}

