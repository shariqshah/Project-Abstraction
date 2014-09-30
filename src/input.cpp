#include "input.h"

namespace Input
{
	namespace
	{
        std::vector<int>releasedKeys;
		const int cNumKeyboardKeys = 283;
		const int cNumMouseButtons = 5;
		const int cMaxDelta = 50;
        static Uint8 sKeyState[cNumKeyboardKeys];
        static Uint8 sMouseButtonState[cNumMouseButtons];
		static Sint32 sMouseX, sMouseY;
		static Sint32 sMouseRelX, sMouseRelY;
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
		
}
