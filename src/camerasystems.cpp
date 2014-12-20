#include "camerasystems.h"
#include "componentmanager.h"
#include "input.h"
#include "transform.h"
#include "gameobject.h"
#include "renderer.h"
#include "utilities.h"

namespace System
{
	namespace CameraSystem
	{
		namespace
		{
			const float cMaxUpDownRot   = 80.f;
			
			float sTotalUpDownRot = 0.f;
			float sMovSpeed       = 20.f;
			float sRotSpeed       = 1.2f;
			float sSprintFactor   = 3.f;
			
			GameObject* activeObject = NULL;
		}

		void setActiveObject(GameObject* gameObject)
		{
			if(GO::hasComponent(gameObject, Component::TRANSFORM) &&
			   GO::hasComponent(gameObject, Component::CAMERA)    &&
			   gameObject->tag == "FreeCamera")
			{
				activeObject = gameObject;
				Log::message("Free camera system initailized with "
							 + activeObject->name);
			}
		}

		void updateFreeCamera(float deltaTime)
		{
			auto transform = CompManager::getTransform(activeObject);
			// if(!Input::isCursorLocked())
			// 	Input::setCursorLock(true);
					
			float increment = sMovSpeed * deltaTime;
			Vec3 translation(0.f);

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
				upDownRot -= sRotSpeed * deltaTime;
			if(Input::isPressed(Input::Key::DOWN))
				upDownRot += sRotSpeed * deltaTime;
				

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

			float epsilon = 0.0001;
			if(upDownRot > epsilon || upDownRot < -epsilon)
			{
				Transform::rotate(transform,
								  Vec3(-1, 0, 0),
								  upDownRot,
								  Transform::Space::LOCAL);
			} 
			
			if(leftRightRot > epsilon || leftRightRot < -epsilon)
			{
				Transform::rotate(transform,
								  Vec3(0, 1, 0),
								  -leftRightRot,
								  Transform::Space::WORLD);
			}
			

			if(translation.x != 0 || translation.y != 0 || translation.z != 0)
				Transform::translate(transform, translation, Transform::Space::LOCAL);
			
			Renderer::addText(Utils::toString(transform->position));
		}
	}
}
