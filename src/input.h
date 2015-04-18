#ifndef _input_H_
#define _input_H_

#include "../include/SDL2/SDL.h"

namespace Input
{
   	enum MouseButton : Uint8
	{
		M_LEFT   = SDL_BUTTON_LEFT,
		M_RIGHT  = SDL_BUTTON_RIGHT,
		M_MIDDLE = SDL_BUTTON_MIDDLE,
		M_X1     = SDL_BUTTON_X1,
		M_X2     = SDL_BUTTON_X2
	};

	enum class KeyState : Uint8
	{
		RELEASED = SDL_RELEASED,
		PRESSED  = SDL_PRESSED,
		INACTIVE = Uint8(3)
	};

	enum Key : SDL_Keycode
	{
		A        = SDLK_a,
		B        = SDLK_b,
		C        = SDLK_c,
		D        = SDLK_d,
		E        = SDLK_e,
		F        = SDLK_f,
		G        = SDLK_g,
		H        = SDLK_h,
		I        = SDLK_i,
		J        = SDLK_j,
		K        = SDLK_k,
		L        = SDLK_l,
		M        = SDLK_m,
		N        = SDLK_n,
		O        = SDLK_o,
		P        = SDLK_p,
		Q        = SDLK_q,
		R        = SDLK_r,
		S        = SDLK_s,
		T        = SDLK_t,
		U        = SDLK_u,
		V        = SDLK_v,
		W        = SDLK_w,
		X        = SDLK_x,
		Y        = SDLK_y,
		Z        = SDLK_z,
		K0       = SDLK_0,
		K1       = SDLK_1,
		K2       = SDLK_2,
		K3       = SDLK_3,
		K4       = SDLK_4,
		K5       = SDLK_5,
		K6       = SDLK_6,
		K7       = SDLK_7,
		K8       = SDLK_8,
		K9       = SDLK_9,
		F1       = SDLK_F1,
		F2       = SDLK_F2,
		F3       = SDLK_F3,
		F4       = SDLK_F4,
		F5       = SDLK_F5,
		F6       = SDLK_F6,
		F7       = SDLK_F7,
		F8       = SDLK_F8,
		F9       = SDLK_F9,
		F10      = SDLK_F10,
		F11      = SDLK_F11,
		F12      = SDLK_F12,
		UP       = SDLK_UP,
		DOWN     = SDLK_DOWN,
		LEFT     = SDLK_LEFT,
		RIGHT    = SDLK_RIGHT,
		LSHIFT   = SDLK_LSHIFT,
		RSHIFT   = SDLK_RSHIFT,
		LCTRL    = SDLK_LCTRL,
		RCTRL    = SDLK_RCTRL,
		RSUPER   = SDLK_RGUI,
		LSUPER   = SDLK_LGUI,
		SPACE    = SDLK_SPACE,
		ENTER    = SDLK_RETURN,
		ESC      = SDLK_ESCAPE,
		TAB      = SDLK_TAB,
		HOME     = SDLK_HOME,
		END      = SDLK_END,
		DELETE   = SDLK_DELETE,
		BACKSPC  = SDLK_BACKSPACE,
		TILDE    = SDLK_BACKQUOTE,
		NP_PLUS  = SDLK_KP_PLUS,
		NP_MINUS = SDLK_KP_MINUS
	};

	bool isPressed(Key key);
	bool isReleased(Key key);
	bool isPressed(MouseButton button);
	bool isReleased(MouseButton button);
	bool isTextInputActive();
	
	void updateKeys(SDL_KeyboardEvent event);
	void updateMouseButtons(SDL_MouseButtonEvent event);
	void updateReleasedKeys();
	void updateMousePostion(SDL_MouseMotionEvent event);
	void updateScroll(SDL_MouseWheelEvent event);
	void textEntered(const char* text);

    void showCursor(bool show);
	void setCursorLock(bool lock);
	bool isCursorVisible();
	bool isCursorLocked();
    void initializeInput();

	Sint32 getMouseX();
	Sint32 getMouseY();
	Sint32 getMouseRelX();
	Sint32 getMouseRelY();
	Sint32 getScrollX();
	Sint32 getScrollY();
	void   resetMouseRel();

    void enableTextInput(bool enable);
	void generateBindings();
}


#endif
