#include "camerasystems.h"

namespace System
{
	namespace CameraSystem
	{
		namespace
		{
			const float cMaxUpDownRot = 80.f;
			const long cValidComponents = ((long)ComponentType::TRANSFORM |
										   (long) ComponentType::CAMERA);
			static float sTotalUpDownRot = 0.f;
			static float sMovSpeed = 20.f;
			static float sRotSpeed = 1.2f;
			static float sSprintFactor = 3.f;
		}

		void updateFreeCamera(float deltaTime, GameObject *gameObject)
		{
			if(gameObject->hasComponents(cValidComponents) &&
			   gameObject->compareTag("FreeCamera"))
			{
				if(!Input::isCursorLocked())
					Input::setCursorLock(true);
				
				auto transform = gameObject->getComponent<Transform>("Transform");
				auto camera = gameObject->getComponent<Camera>("Camera");
				
				float increment = sMovSpeed * deltaTime;
				glm::vec3 translation(0.f);

				float upDownRot = 0.f; 
				float leftRightRot = 0.f;

				upDownRot = Input::getMouseRelY() * sRotSpeed * deltaTime;
				leftRightRot = Input::getMouseRelX() * sRotSpeed * deltaTime;
				
				if(Input::isPressed(Input::Key::W))
					translation.z -= increment;
				if(Input::isPressed(Input::Key::S))
					translation.z += increment;
				if(Input::isPressed(Input::Key::A))
					translation.x -= increment;
				if(Input::isPressed(Input::Key::D))
					translation.x += increment;
				if(Input::isPressed(Input::Key::Q))
					translation.y += increment;
				if(Input::isPressed(Input::Key::E))
					translation.y -= increment;
				if(Input::isPressed(Input::Key::LSHIFT))
					translation *= sSprintFactor;

				if(Input::isPressed(Input::Key::LEFT))
					leftRightRot -= sRotSpeed * deltaTime;
				if(Input::isPressed(Input::Key::RIGHT))
					leftRightRot += sRotSpeed * deltaTime;

				if(Input::isPressed(Input::Key::UP))
					upDownRot += sRotSpeed * deltaTime;
				if(Input::isPressed(Input::Key::DOWN))
					upDownRot -= sRotSpeed * deltaTime;
				

				leftRightRot = glm::degrees(leftRightRot);
				upDownRot = glm::degrees(upDownRot);
				sTotalUpDownRot += upDownRot;
			    
				if(sTotalUpDownRot >= cMaxUpDownRot)
				{
					sTotalUpDownRot = cMaxUpDownRot;
					upDownRot = 0.f;
				}
				else if(sTotalUpDownRot <= -cMaxUpDownRot)
				{
					sTotalUpDownRot = -cMaxUpDownRot;
					upDownRot = 0.f;
				}

				transform->rotate(glm::vec3(-1, 0, 0),
								  upDownRot,
								  Transform::Space::LOCAL);
				
				transform->rotate(glm::vec3(0, 1, 0),
								  -leftRightRot,
								  Transform::Space::WORLD);

				if(translation.x != 0 || translation.y != 0 || translation.z != 0)
					transform->translate(translation, Transform::Space::LOCAL);

				Renderer::addText(Utils::toString(transform->getPosition()));
			}
		}
	}
}
