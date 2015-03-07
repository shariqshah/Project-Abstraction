#ifndef GUI_H
#define GUI_H

#include "../include/imgui/imgui.h"
#include "mathdefs.h"

namespace Gui
{
	enum WindowFlags
	{
		WF_Default           = 0,
		WF_NoTitleBar        = ImGuiWindowFlags_NoTitleBar,
		WF_NoResize          = ImGuiWindowFlags_NoResize,
		WF_NoMove            = ImGuiWindowFlags_NoMove,
		WF_NoScrollbar       = ImGuiWindowFlags_NoScrollbar,
		WF_NoScrollWithMouse = ImGuiWindowFlags_NoScrollWithMouse,
		WF_NoCollapse        = ImGuiWindowFlags_NoCollapse,
		WF_AlwaysAutoResize  = ImGuiWindowFlags_AlwaysAutoResize,
		WF_ShowBorders       = ImGuiWindowFlags_ShowBorders,
		WF_NoSavedSettings   = ImGuiWindowFlags_NoSavedSettings
	};
	
	void initialize();
	void cleanup();
	void update(float deltaTime);
	void render();
	void resize();
	void generateBindings();
	void updateKeyDown(uint8_t key, bool isDown, bool modCtrl, bool modShift);
	void updateMouseWheel(float scroll);
	void textEntered(const char* text);

	// Widgets
	void text(const char* string);
	bool button(const char* string);
	bool colorEdit3(const char* text, Vec3* color);
	bool colorEdit4(const char* text, Vec4* color, bool showAlpha = true);
	bool inputfloat(const char* text, float* value);
	bool inputVec2(const char* text, Vec2* value);
	bool inputVec3(const char* text, Vec3* value);
	bool inputVec4(const char* text, Vec4* value);
	bool begin(const char* name,
			   bool        opened,
			   Vec2        size = Vec2(0),
			   float       alpha = 0.85f,
			   WindowFlags flags = WF_Default);
	void end();
}

#endif
