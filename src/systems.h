#ifndef _systems_H
#define _systems_H

#include "scenemanager.h"
#include "componentmanager.h"
#include "scriptengine.h"
#include "input.h"
#include "camerasystems.h"

namespace System
{
	void initialize();
	void update(float deltaTime, GameObject* gameObject);
	void update(float deltaTime);
	void cleanup();
}

#endif
