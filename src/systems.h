#ifndef _systems_H
#define _systems_H

#include "scenemanager.h"
#include "camerasystems.h"
#include "light.h"
#include "model.h"
#include "rigidbody.h"

namespace System
{
	void update(float deltaTime, GameObject *gameObject);
	void update(float deltaTime);
}

#endif
