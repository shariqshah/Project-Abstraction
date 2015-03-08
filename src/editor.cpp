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

		int  currentItem = 1;
		
		const float OPACITY  = 0.9f;
		const int   BUF_SIZE = 128;

		char inputName[BUF_SIZE] = "";
		char inputTag[BUF_SIZE]  = "";

		GameObject* selectedGO = NULL;
		CTransform* transform  = NULL;
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
				ImGui::Begin(selectedGO->name.c_str(), &showSelectedGO, Vec2(400, 400), OPACITY);

				if(ImGui::InputText("Name", &inputName[0], BUF_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					selectedGO->name = inputName;
				}

				if(ImGui::InputText("Tag", &inputTag[0], BUF_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					selectedGO->tag = inputTag;
				}

				// Transform Component
				ImGui::Separator();
				ImGui::Text("Transform");

				if(ImGui::InputFloat3("Position", glm::value_ptr(transform->position)))
				{
					Transform::setPosition(transform, transform->position);
				}

				if(ImGui::InputFloat3("Scale", glm::value_ptr(transform->scale)))
				{
					Transform::setScale(transform, transform->scale);
				}

				Vec3 eulerAngles = glm::eulerAngles(transform->rotation);
				eulerAngles = glm::degrees(eulerAngles);
				if(ImGui::InputFloat3("Rotation", glm::value_ptr(eulerAngles)))
				{
					eulerAngles = glm::radians(eulerAngles);
					Quat newRotation(eulerAngles);
					Transform::setRotation(transform, newRotation);
				}

				ImGui::Text("Add Toggles for switching transformation space between world and local");
				
				ImGui::End();
			}
		}

		
		ImGui::ShowTestWindow(&showLog);
	}
	
	void cleanup()
	{
		
	}
}
