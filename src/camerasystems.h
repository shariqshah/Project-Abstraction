#ifndef _camerasystems_h
#define _camerasystems_h

#include "componentmanager.h"
#include "input.h"

namespace System
{
	namespace CameraSystem
	{
		void updateFreeCamera(float deltaTime);
		void setActiveObject(GameObject* gameObject);
    }
}

#endif
