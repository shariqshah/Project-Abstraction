#ifndef _camerasystems_h
#define _camerasystems_h

struct GameObject;

namespace System
{
	namespace CameraSystem
	{
		void updateFreeCamera(float deltaTime);
		void setActiveObject(GameObject* gameObject);
    }
}

#endif
