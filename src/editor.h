#ifndef _editor_H
#define _editor_H

namespace Editor
{
	void initialize();
	void update(float deltaTime, bool* quit);
	void cleanup();
	void setUpdateTime(const float time);
	void setDrawTime(const float time);
	void addDebugFloat(const char* description, float value);
	void addDebugInt(const char* description, int value);
	void addDebugTexture(const char* description, int texture);
}

#endif
