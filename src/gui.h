#ifndef GUI_H
#define GUI_H

#include "../include/imgui/imgui.h"
#include "mathdefs.h"

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

namespace Gui
{	
	void initialize();
	void cleanup();
	void update(float deltaTime);
	void render();
	void resize();
	void generateBindings();
	void updateKeyDown(uint8_t key, bool isDown, bool modCtrl, bool modShift);
	void updateMouseWheel(float scroll);
	void textEntered(const char* text);
}

#endif
