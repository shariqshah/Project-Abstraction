#ifndef _input_H_
#define _input_H_

#include <vector>

#include "../include/SDL2/SDL.h"

namespace Input
{
   	enum class MouseButton : Uint8
	{
		LEFT   = SDL_BUTTON_LEFT,
		RIGHT  = SDL_BUTTON_RIGHT,
		MIDDLE = SDL_BUTTON_MIDDLE,
		X1     = SDL_BUTTON_X1,
		X2     = SDL_BUTTON_X2
	};

	enum class KeyState : Uint8
	{
		RELEASED = SDL_RELEASED,
		PRESSED  = SDL_PRESSED,
		INACTIVE = Uint8(3)
	};

	enum class Key : SDL_Keycode
	{
		K0     = SDLK_0,
		K1     = SDLK_1,
		K2     = SDLK_2,
		K3     = SDLK_3,
		K4     = SDLK_4,
		K5     = SDLK_5,
		K6     = SDLK_6,
		K7     = SDLK_7,
		K8     = SDLK_8,
		K9     = SDLK_9,
		A      = SDLK_a,
		B      = SDLK_b,
		C      = SDLK_c,
		D      = SDLK_d,
		E      = SDLK_e,
		F      = SDLK_f,
		G      = SDLK_g,
		H      = SDLK_h,
		I      = SDLK_i,
		J      = SDLK_j,
		K      = SDLK_k,
		L      = SDLK_l,
		M      = SDLK_m,
		N      = SDLK_n,
		O      = SDLK_o,
		P      = SDLK_p,
		Q      = SDLK_q,
		R      = SDLK_r,
		S      = SDLK_s,
		T      = SDLK_t,
		U      = SDLK_u,
		V      = SDLK_v,
		W      = SDLK_w,
		X      = SDLK_x,
		Y      = SDLK_y,
		Z      = SDLK_z,
		UP     = SDLK_UP,
		DOWN   = SDLK_DOWN,
		LEFT   = SDLK_LEFT,
		RIGHT  = SDLK_RIGHT,
		LSHIFT = SDLK_LSHIFT,
		RSHIFT = SDLK_RSHIFT,
		LCTRL  = SDLK_LCTRL,
		RCTRL  = SDLK_RCTRL,
		SPACE  = SDLK_SPACE
	};

	bool isPressed(Key key);
	bool isReleased(Key key);
	bool isPressed(MouseButton button);
	bool isReleased(MouseButton button);
	
	void updateKeys(SDL_KeyboardEvent event);
	void updateMouseButtons(SDL_MouseButtonEvent event);
	void updateReleasedKeys();
	void updateMousePostion(SDL_MouseMotionEvent event);

    void showCursor(bool show);
	void setCursorLock(bool lock);
	bool isCursorVisible();
	bool isCursorLocked();
    void initializeInput();

	Sint32 getMouseX();
	Sint32 getMouseY();
	Sint32 getMouseRelX();
	Sint32 getMouseRelY();
	void resetMouseRel();
}


#endif
