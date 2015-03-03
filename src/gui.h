#ifndef GUI_H
#define GUI_H

namespace Gui
{
	void initialize();
	void cleanup();
	void update(float deltaTime);
	void render();

	// Widgets
	void text(const char* string);
	bool button(const char* string);
}

#endif
