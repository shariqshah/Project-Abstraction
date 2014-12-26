#ifndef _systems_H
#define _systems_H

struct GameObject;

namespace System
{
	void initialize();
	void update(float deltaTime, GameObject* gameObject);
	void update(float deltaTime, bool* quit);
	void cleanup();
}

#endif
