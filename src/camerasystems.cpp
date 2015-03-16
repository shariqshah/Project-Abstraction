#include "camerasystems.h"
#include "componentmanager.h"
#include "input.h"
#include "transform.h"
#include "gameobject.h"
#include "renderer.h"
#include "utilities.h"
#include "scenemanager.h"
#include "camera.h"

namespace System
{
	namespace CameraSystem
	{
		namespace
		{
			const float maxUpDownRot = 80.f;
			const float epsilon = 0.0001;
			
			float sTotalUpDownRot = 0.f;
			float sMovSpeed       = 20.f;
			float sRotSpeed       = 1.2f;
			float sSprintFactor   = 3.f;
			
			Node activeObject;
		}

		void setActiveObject(GameObject* gameObject)
		{
			if(GO::hasComponent(gameObject, Component::TRANSFORM) &&
			   GO::hasComponent(gameObject, Component::CAMERA)    &&
			   gameObject->tag == "FreeCamera")
			{
				activeObject = gameObject->node;
				Log::message("Free camera system initialized with " + gameObject->name);
			}
		}

		void updateFreeCamera(float deltaTime)
		{
			GameObject* activeObjectPtr = SceneManager::find(activeObject);
			auto transform = GO::getTransform(activeObjectPtr);
			auto camera    = Renderer::Camera::getCameraAtIndex(activeObjectPtr->compIndices[(int)Component::CAMERA]);
			
			// if(!Input::isCursorLocked())
			// 	Input::setCursorLock(true);
					
			float increment = sMovSpeed * deltaTime;
			Vec3 translation(0.f);

			float upDownRot = 0.f; 
			float leftRightRot = 0.f;

			if(Input::isPressed(Input::MouseButton::M_RIGHT))
			{
				upDownRot    = Input::getMouseRelY() * sRotSpeed * deltaTime;
				leftRightRot = Input::getMouseRelX() * sRotSpeed * deltaTime;
			}

				
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

			if(Input::isPressed(Input::Key::J))
				leftRightRot -= sRotSpeed * deltaTime;
			if(Input::isPressed(Input::Key::L))
				leftRightRot += sRotSpeed * deltaTime;

			if(Input::isPressed(Input::Key::I))
				upDownRot -= sRotSpeed * deltaTime;
			if(Input::isPressed(Input::Key::K))
				upDownRot += sRotSpeed * deltaTime;
			
			leftRightRot = glm::degrees(leftRightRot);
			upDownRot = glm::degrees(upDownRot);
			sTotalUpDownRot += upDownRot;
			    
			if(sTotalUpDownRot >= maxUpDownRot)
			{
				sTotalUpDownRot = maxUpDownRot;
				upDownRot = 0.f;
			}
			else if(sTotalUpDownRot <= -maxUpDownRot)
			{
				sTotalUpDownRot = -maxUpDownRot;
				upDownRot = 0.f;
			}
			if(upDownRot > epsilon || upDownRot < -epsilon)
			{
				Transform::rotate(transform,
								  Vec3(-1, 0, 0),
								  upDownRot,
								  Transform::Space::TS_LOCAL);
			} 
			if(leftRightRot > epsilon || leftRightRot < -epsilon)
			{
				Transform::rotate(transform,
								  Vec3(0, 1, 0),
								  -leftRightRot,
								  Transform::Space::TS_WORLD);
			}
			if(translation.x != 0 || translation.y != 0 || translation.z != 0)
			{
				Transform::translate(transform, translation, Transform::Space::TS_LOCAL);
				Log::message("Player : " + Utils::toString(transform->position));
			}
		}
	}
}
