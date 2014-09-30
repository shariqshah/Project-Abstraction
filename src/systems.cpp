#include "systems.h"

namespace System
{
	void update(float deltaTime, GameObject *gameObject)
	{
		System::CameraSystem::updateFreeCamera(deltaTime, gameObject);

		if(gameObject->hasComponents((long)ComponentType::LIGHT))
		{
			auto transform = gameObject->getComponent<Transform>("Transform");
			auto light = gameObject->getComponent<Light>("Light");
			
			float increment = 5.f * deltaTime;
			glm::vec3 translation(0.f);
				
			if(Input::isPressed(Input::Key::I))
				translation.z -= increment;
			if(Input::isPressed(Input::Key::K))
				translation.z += increment;
			if(Input::isPressed(Input::Key::J))
				translation.x -= increment;
			if(Input::isPressed(Input::Key::L))
				translation.x += increment;
			if(Input::isPressed(Input::Key::U))
				translation.y += increment;
			if(Input::isPressed(Input::Key::O))
				translation.y -= increment;

			if(translation.x != 0 || translation.y != 0 || translation.z != 0)
				transform->translate(translation, Transform::Space::LOCAL);

			if(Input::isPressed(Input::Key::M))
				transform->rotate(glm::vec3(1, 0, 0), 5 * deltaTime);
			if(Input::isPressed(Input::Key::N))
				transform->rotate(glm::vec3(1, 0, 0), -5 * deltaTime);

			if(Input::isReleased(Input::Key::G))
				light->setShadowMapCount(1);
			if(Input::isReleased(Input::Key::H))
				light->setShadowMapCount(0);

			if(Input::isReleased(Input::Key::K1))
				light->setFov(90);
			if(Input::isReleased(Input::Key::K2))
				light->setFov(180);
			if(Input::isReleased(Input::Key::K3))
				light->setFov(270);
			if(Input::isReleased(Input::Key::K4))
				light->setFov(360);

			if(Input::isReleased(Input::Key::V))
			{
				if(gameObject->hasComponents((long)ComponentType::MODEL))
					gameObject->removeComponent("Model");
			}
		}
		
		auto transform = gameObject->getComponent<Transform>("Transform");
		if(transform->needsSync())
		{
			Renderer::setNodeTransform(gameObject->getNode(),
									   transform->getModelMatrix());
			transform->setSynced();
		}
	}
}
