#include "systems.h"

namespace System
{
	void update(float deltaTime, GameObject *gameObject)
	{
		auto transform = gameObject->getComponent<Transform>("Transform");
		if(transform->needsSync())
		{
			Renderer::setNodeTransform(gameObject->getNode(),
									   transform->getModelMatrix());
			transform->setSynced();
		}
		
		System::CameraSystem::updateFreeCamera(deltaTime, gameObject);
	}
}
