#ifndef _camerasystems_h
#define _camerasystems_h

#include "gameobject.h"
#include "transform.h"
#include "camera.h"
#include "input.h"
#include "utilities.h"

namespace System
{
	namespace CameraSystem
	{
		void updateFreeCamera(float deltaTime, GameObject *gameObject);
    }
}

#endif
