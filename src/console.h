#ifndef _console_H
#define _console_H

#include <iostream>

namespace Console
{
	enum MessageType { MSG_NORM = 0, MSG_WARNING, MSG_ERROR, MSG_COMMAND };
	void initialize();
	void addMessage(MessageType type, const std::string& message);
	void toggle();
	void showWindow(bool show);
	void cleanup();
	void update();
}

#endif
