#include <vector>
#include <algorithm>
#include "scriptengine.h"
#include "input.h"
#include "gui.h"
#include "passert.h"

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
		asIScriptEngine* engine = ScriptEngine::getEngine();
		int rc = engine->RegisterEnum("Key"); PA_ASSERT(rc >= 0);
		engine->RegisterEnumValue("Key", "A", (int)Key::A);
		engine->RegisterEnumValue("Key", "B", (int)Key::B);
		engine->RegisterEnumValue("Key", "C", (int)Key::C);
		engine->RegisterEnumValue("Key", "D", (int)Key::D);
		engine->RegisterEnumValue("Key", "E", (int)Key::E);
		engine->RegisterEnumValue("Key", "F", (int)Key::F);
		engine->RegisterEnumValue("Key", "G", (int)Key::G);
		engine->RegisterEnumValue("Key", "H", (int)Key::H);
		engine->RegisterEnumValue("Key", "I", (int)Key::I);
		engine->RegisterEnumValue("Key", "J", (int)Key::J);
		engine->RegisterEnumValue("Key", "K", (int)Key::K);
		engine->RegisterEnumValue("Key", "L", (int)Key::L);
		engine->RegisterEnumValue("Key", "M", (int)Key::M);
		engine->RegisterEnumValue("Key", "N", (int)Key::N);
		engine->RegisterEnumValue("Key", "O", (int)Key::O);
		engine->RegisterEnumValue("Key", "P", (int)Key::P);
		engine->RegisterEnumValue("Key", "Q", (int)Key::Q);
		engine->RegisterEnumValue("Key", "R", (int)Key::R);
		engine->RegisterEnumValue("Key", "S", (int)Key::S);
		engine->RegisterEnumValue("Key", "T", (int)Key::T);
		engine->RegisterEnumValue("Key", "U", (int)Key::U);
		engine->RegisterEnumValue("Key", "V", (int)Key::V);
		engine->RegisterEnumValue("Key", "W", (int)Key::W);
		engine->RegisterEnumValue("Key", "X", (int)Key::X);
		engine->RegisterEnumValue("Key", "Y", (int)Key::Y);
		engine->RegisterEnumValue("Key", "Z", (int)Key::Z);
		engine->RegisterEnumValue("Key", "K0", (int)Key::K0);
		engine->RegisterEnumValue("Key", "K1", (int)Key::K1);
		engine->RegisterEnumValue("Key", "K2", (int)Key::K2);
		engine->RegisterEnumValue("Key", "K3", (int)Key::K3);
		engine->RegisterEnumValue("Key", "K4", (int)Key::K4);
		engine->RegisterEnumValue("Key", "K5", (int)Key::K5);
		engine->RegisterEnumValue("Key", "K6", (int)Key::K6);
		engine->RegisterEnumValue("Key", "K7", (int)Key::K7);
		engine->RegisterEnumValue("Key", "K8", (int)Key::K8);
		engine->RegisterEnumValue("Key", "K9", (int)Key::K9);
		engine->RegisterEnumValue("Key", "F1", (int)Key::F1);
		engine->RegisterEnumValue("Key", "F2", (int)Key::F2);
		engine->RegisterEnumValue("Key", "F3", (int)Key::F3);
		engine->RegisterEnumValue("Key", "F4", (int)Key::F4);
		engine->RegisterEnumValue("Key", "F5", (int)Key::F5);
		engine->RegisterEnumValue("Key", "F6", (int)Key::F6);
		engine->RegisterEnumValue("Key", "F7", (int)Key::F7);
		engine->RegisterEnumValue("Key", "F8", (int)Key::F8);
		engine->RegisterEnumValue("Key", "F9", (int)Key::F9);
		engine->RegisterEnumValue("Key", "F10", (int)Key::F10);
		engine->RegisterEnumValue("Key", "F11", (int)Key::F11);
		engine->RegisterEnumValue("Key", "F12", (int)Key::F12);
		engine->RegisterEnumValue("Key", "UP", (int)Key::UP);
		engine->RegisterEnumValue("Key", "DOWN", (int)Key::DOWN);
		engine->RegisterEnumValue("Key", "LEFT", (int)Key::LEFT);
		engine->RegisterEnumValue("Key", "RIGHT", (int)Key::RIGHT);
		engine->RegisterEnumValue("Key", "LSHIFT", (int)Key::LSHIFT);
		engine->RegisterEnumValue("Key", "RSHIFT", (int)Key::RSHIFT);
		engine->RegisterEnumValue("Key", "LCTRL", (int)Key::LCTRL);
		engine->RegisterEnumValue("Key", "RCTRL", (int)Key::RCTRL);
		engine->RegisterEnumValue("Key", "RSUPER", (int)Key::RSUPER);
		engine->RegisterEnumValue("Key", "LSUPER", (int)Key::LSUPER);
		engine->RegisterEnumValue("Key", "SPACE", (int)Key::SPACE);
		engine->RegisterEnumValue("Key", "ENTER", (int)Key::ENTER);
		engine->RegisterEnumValue("Key", "ESC", (int)Key::ESC);
		engine->RegisterEnumValue("Key", "TAB", (int)Key::TAB);
		engine->RegisterEnumValue("Key", "HOME", (int)Key::HOME);
		engine->RegisterEnumValue("Key", "END", (int)Key::END);
		engine->RegisterEnumValue("Key", "DELETE", (int)Key::DELETE);
		engine->RegisterEnumValue("Key", "BACKSPC", (int)Key::BACKSPC);
		engine->RegisterEnumValue("Key", "NP_PLUS", (int)Key::NP_PLUS);
		engine->RegisterEnumValue("Key", "NP_MINUS", (int)Key::NP_MINUS);

		rc = engine->RegisterEnum("MouseButton"); PA_ASSERT(rc >= 0);
		engine->RegisterEnumValue("MouseButton", "LEFT",   MouseButton::M_LEFT);
		engine->RegisterEnumValue("MouseButton", "RIGHT",  MouseButton::M_RIGHT);
		engine->RegisterEnumValue("MouseButton", "MIDDLE", MouseButton::M_MIDDLE);
		engine->RegisterEnumValue("MouseButton", "X1",     MouseButton::M_X1);
		engine->RegisterEnumValue("MouseButton", "X2",     MouseButton::M_X2);

		engine->SetDefaultNamespace("Input");
		engine->RegisterGlobalFunction("int32 getMouseX()", asFUNCTION(getMouseX), asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->RegisterGlobalFunction("int32 getMouseY()", asFUNCTION(getMouseY), asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->RegisterGlobalFunction("int32 getMouseRelX()", asFUNCTION(getMouseRelX), asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->RegisterGlobalFunction("int32 getMouseRelY()", asFUNCTION(getMouseRelY), asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->RegisterGlobalFunction("bool isPressed(Key)",
									   asFUNCTIONPR(isPressed, (Key), bool),
									   asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->RegisterGlobalFunction("bool isPressed(MouseButton)",
									   asFUNCTIONPR(isPressed, (MouseButton), bool),
									   asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->RegisterGlobalFunction("bool isReleased(Key)",
									   asFUNCTIONPR(isReleased, (Key), bool),
									   asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->RegisterGlobalFunction("bool isReleased(MouseButton)",
									   asFUNCTIONPR(isReleased, (MouseButton), bool),
									   asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->RegisterGlobalFunction("int32 getMouseScrollX()", asFUNCTION(getScrollX), asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->RegisterGlobalFunction("int32 getMouseScrollY()", asFUNCTION(getScrollY), asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->RegisterGlobalFunction("void showCursor(bool)", asFUNCTION(showCursor), asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->RegisterGlobalFunction("void lockCursor(bool)", asFUNCTION(setCursorLock), asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->RegisterGlobalFunction("bool isCursorVisible()", asFUNCTION(isCursorVisible), asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->RegisterGlobalFunction("bool isCursorLocked()", asFUNCTION(isCursorLocked), asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->SetDefaultNamespace("");
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
