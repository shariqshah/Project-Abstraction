#include "camerasystems.h"
#include "input.h"
#include "transform.h"
#include "gameobject.h"
#include "renderer.h"
#include "utilities.h"
#include "scenemanager.h"
#include "camera.h"
#include "passert.h"

#include "../include/imgui/imgui.h"

namespace System
{
	namespace CameraSystem
	{
		namespace
		{
			const float maxUpDownRot = 80.f;
			const float epsilon = 0.f;//0.0001;
			float totalUpDownRot = 0.f;
			float movSpeed       = 20.f;
			float rotSpeed       = 0.5f;
			float sprintFactor   = 3.f;			
			Node  activeObject;
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
			CTransform* transform       = GO::getTransform(activeObjectPtr);					
			float increment = movSpeed * deltaTime;
			Vec3 translation(0.f);

			float upDownRot = 0.f; 
			float leftRightRot = 0.f;

			if(Input::isPressed(Input::MouseButton::M_RIGHT))
			{
				Input::showCursor(false);
				Input::setCursorLock(true);

				// if(Input::getMouseRelX() > 0) leftRightRot += rotSpeed * deltaTime;
				// if(Input::getMouseRelX() < 0) leftRightRot -= rotSpeed * deltaTime;

				// if(Input::getMouseRelY() > 0) upDownRot += rotSpeed * deltaTime;
				// if(Input::getMouseRelY() < 0) upDownRot -= rotSpeed * deltaTime;
				
				upDownRot    = (float)Input::getMouseRelY() * rotSpeed;
				leftRightRot = (float)Input::getMouseRelX() * rotSpeed;
			}

			if(Input::isReleased(Input::MouseButton::M_RIGHT))
			{
				Input::showCursor(true);
				Input::setCursorLock(false);
			}
				
			if(Input::isPressed(Input::Key::W))      translation.z -= increment;
			if(Input::isPressed(Input::Key::S))	     translation.z += increment;
			if(Input::isPressed(Input::Key::A))	     translation.x -= increment;
			if(Input::isPressed(Input::Key::D))	     translation.x += increment;
			if(Input::isPressed(Input::Key::Q))	     translation.y += increment;
			if(Input::isPressed(Input::Key::E))	     translation.y -= increment;
			if(Input::isPressed(Input::Key::LSHIFT)) translation *= sprintFactor;

			if(Input::isPressed(Input::Key::J))		 leftRightRot -= rotSpeed;
			if(Input::isPressed(Input::Key::L))		 leftRightRot += rotSpeed;
			
			if(Input::isPressed(Input::Key::I))      upDownRot -= rotSpeed;
			if(Input::isPressed(Input::Key::K))      upDownRot += rotSpeed;
			
			// leftRightRot = glm::degrees(leftRightRot);
			// upDownRot    = glm::degrees(upDownRot);
			totalUpDownRot += upDownRot;
			    
			if(totalUpDownRot >= maxUpDownRot)
			{
				totalUpDownRot = maxUpDownRot;
				upDownRot = 0.f;
			}
			else if(totalUpDownRot <= -maxUpDownRot)
			{
				totalUpDownRot = -maxUpDownRot;
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

			ImGui::Begin("Debug Movment Values");
			ImGui::Text("Updown : %f", upDownRot);
			ImGui::Text("LeftRight : %f", leftRightRot);
			ImGui::End();
		}
	}
}
