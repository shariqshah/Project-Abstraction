#include "editor.h"
#include "gui.h"
#include "scenemanager.h"
#include "transform.h"
#include "light.h"
#include "camera.h"
#include "model.h"
#include "material.h"
#include "gameobject.h"
#include "log.h"

namespace Editor
{
	namespace
	{
		bool showMainToolBar  = true;
		bool showLog          = false;
		bool showSceneObjects = false;
		bool showSelectedGO   = false;
		bool tsWorld          = true;
		bool tsLocal          = false;

		int  currentItem = 1;
		
		const float OPACITY  = 0.85f;
		const int   BUF_SIZE = 128;

		char inputName[BUF_SIZE] = "";
		char inputTag[BUF_SIZE]  = "";

		GameObject* selectedGO = NULL;
		CTransform* transform  = NULL;
		int transformSpace     = Transform::TS_WORLD;
	}
	
	void initialize()
	{
		
	}

	bool selectGameObject(void* gameObjects, int index, const char** name)
	{
		GameObject* gameObject = SceneManager::find(index);
		*name = gameObject->name.c_str();
		return true;
	}
	
	void update(float deltaTime, bool* quit)
	{
		ImGui::Begin("Tools", &showMainToolBar, Vec2(100, 20), OPACITY, (WF_NoTitleBar | WF_NoCollapse));

		if(ImGui::Button("Log"))   showLog = !showLog;                   ImGui::SameLine();
		if(ImGui::Button("Scene")) showSceneObjects = !showSceneObjects; ImGui::SameLine();

		ImGui::PushID("Exit_Button");
		Vec4 exitColor(0.89f, 0.05f, 0.17f, 1.f);
		ImGui::PushStyleColor(ImGuiCol_Button, exitColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, exitColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, exitColor);
		if(ImGui::Button("Exit")) *quit = true;
		ImGui::PopStyleColor(3);
		ImGui::PopID();
		
		ImGui::End();

		// Selectable list of gameobjects currently in the scene
		if(showSceneObjects)
		{
			ImGui::Begin("SceneObjects", &showSceneObjects, Vec2(400, 400), OPACITY);
			std::vector<Node>* gameObjects = SceneManager::getSceneObjects();
			if(ImGui::ListBox("GameObjects", &currentItem, &selectGameObject, gameObjects, gameObjects->size(), 10))
			{
				// Selected gameobject changed so update all pointers
				showSelectedGO = true;
				selectedGO = SceneManager::find(currentItem);
				memset(&inputName[0], '\0', BUF_SIZE);
				int copySize = selectedGO->name.size() > BUF_SIZE ? BUF_SIZE : selectedGO->name.size();
				memcpy(&inputName[0], &selectedGO->name[0], copySize);
				copySize = selectedGO->tag.size() > BUF_SIZE ? BUF_SIZE : selectedGO->tag.size();
				memcpy(&inputTag[0], &selectedGO->tag[0], copySize);

				transform = GO::getTransform(selectedGO);
			}
			ImGui::End();

			// Show currently selected gameobject, if any
			if(showSelectedGO)
			{				
				ImGui::Begin(selectedGO->name.c_str(), &showSelectedGO, Vec2(600, 400), OPACITY);

				if(ImGui::InputText("Name", &inputName[0], BUF_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					selectedGO->name = inputName;
				}

				if(ImGui::InputText("Tag", &inputTag[0], BUF_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					selectedGO->tag = inputTag;
				}

				// Transform Component
				if(ImGui::CollapsingHeader("Transform"))
				{
					float step     = 1.f;
					float stepFast = 10.f;

					float width = ImGui::GetWindowWidth() / 5.f;
					ImGui::PushItemWidth(width);
					// Position
					bool updatePosition = false;
					if(ImGui::InputFloat("X", &transform->position.x, step, stepFast))
						updatePosition = true;
					
					ImGui::SameLine();
					if(ImGui::InputFloat("Y", &transform->position.y, step, stepFast))
						updatePosition = true;
					
					ImGui::SameLine();
					if(ImGui::InputFloat("Z", &transform->position.z, step, stepFast))
						updatePosition = true;

					if(updatePosition)
						Transform::setPosition(transform, transform->position);

					ImGui::SameLine();
					ImGui::Text("Position");

					// Scale
					bool updateScale = false;
					if(ImGui::InputFloat("X", &transform->scale.x, step, stepFast))
						updateScale = true;
					
					ImGui::SameLine();
					if(ImGui::InputFloat("Y", &transform->scale.y, step, stepFast))
						updateScale = true;
					
					ImGui::SameLine();
					if(ImGui::InputFloat("Z", &transform->scale.z, step, stepFast))
						updateScale = true;

					if(updateScale)
						Transform::setPosition(transform, transform->scale);

					ImGui::SameLine();
					ImGui::Text("Scale");

					// Rotation
					Vec3 eulerAngles = glm::eulerAngles(transform->rotation);
					eulerAngles = glm::degrees(eulerAngles);
					bool updateRotation = false;
					if(ImGui::InputFloat("X", &eulerAngles.x, step, stepFast))
						updateRotation = true;
					
					ImGui::SameLine();
					if(ImGui::InputFloat("Y", &eulerAngles.y, step, stepFast))
						updateRotation = true;

					ImGui::SameLine();
					if(ImGui::InputFloat("Z", &eulerAngles.z, step, stepFast))
						updateRotation = true;

					if(updateRotation)
					{
						eulerAngles = glm::radians(eulerAngles);
						Quat newRotation(eulerAngles);
						Transform::setRotation(transform, newRotation);
					}
					ImGui::SameLine();
					ImGui::Text("Rotation");
					ImGui::PopItemWidth();
				}				
				ImGui::End();
			}
		}

		
		ImGui::ShowTestWindow(&showLog);
	}
	
	void cleanup()
	{
		
	}
}
