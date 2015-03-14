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
#include "texture.h"

namespace Editor
{
	namespace
	{
		bool showMainToolBar      = true;
		bool showLog              = false;
		bool showSceneObjects     = false;
		bool showSelectedGO       = false;
		bool showRendererSettings = false;
		bool showScriptingWindow  = false;

		int  currentItem = 1;
		
		const float OPACITY  = 0.75f;
		const int   BUF_SIZE = 128;

		char inputName[BUF_SIZE]       = "";
		char inputNewName[BUF_SIZE]    = "";
		char inputTag[BUF_SIZE]        = "";
		char inputModelName[BUF_SIZE]  = "";
		char inputModelTex[BUF_SIZE]   = "";

		GameObject* selectedGO = NULL;
		CTransform* transform  = NULL;
		CLight*     light      = NULL;
		CModel*     model      = NULL;
		CCamera*    camera     = NULL;
	}
	
	void initialize()
	{		
	}

	void updateComponentViewers()
	{
		transform = GO::getTransform(selectedGO);
		// Check light
		if(GO::hasComponent(selectedGO, Component::LIGHT))
			light = GO::getLight(selectedGO);
		else
			light = NULL;
		// Check Model
		if(GO::hasComponent(selectedGO, Component::MODEL))
		{
			model = GO::getModel(selectedGO);
			memset(&inputModelName[0], '\0', BUF_SIZE);
			memset(&inputModelTex[0], '\0', BUF_SIZE);
			size_t copySize = model->filename.size() > BUF_SIZE ? BUF_SIZE : model->filename.size();
			strncpy(&inputModelName[0], model->filename.c_str(), copySize);
			if(model->material == MAT_UNSHADED_TEXTURED || model->material == MAT_PHONG_TEXTURED)
			{
				const char* textureName = Texture::getFilename(model->materialUniforms.texture);
				if(textureName)
				{
					size_t textureNameLen = strlen(textureName);
					copySize = textureNameLen > BUF_SIZE ? BUF_SIZE : textureNameLen;
					strncpy(&inputModelTex[0], textureName, copySize);
				}
				else
				{
					strcpy(&inputModelTex[0], "NONE");
				}
			}
		}
		else
		{
			model = NULL;
		}
		// Check camera
		if(GO::hasComponent(selectedGO, Component::CAMERA))
			camera = GO::getCamera(selectedGO);
		else
			camera = NULL;
	}

	bool selectGameObject(void* gameObjects, int index, const char** name)
	{
		std::vector<Node>* nodeList = (std::vector<Node>*)gameObjects;
		Node node = nodeList->at(index);
		GameObject* gameObject = SceneManager::find(node);
		if(gameObject)
		{
			*name = gameObject->name.c_str();
			return true;
		}
		else
		{
			return false;
		}
	}

	void displayTransform()
	{
		if(ImGui::CollapsingHeader("Transform", "TransformComponent", true, true))
		{
			float step     = 1.f;
			float stepFast = 10.f;
			float width = ImGui::GetWindowWidth() / 5.f;
			ImGui::PushItemWidth(width);
			// Position
			bool updatePosition = false;
			ImGui::PushID("Position");
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
			ImGui::PopID();

			// Rotation
			Vec3 eulerAngles = glm::eulerAngles(transform->rotation);
			eulerAngles = glm::degrees(eulerAngles);
			bool updateRotation = false;
			ImGui::PushID("Rotation");
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
			ImGui::PopID();
					
			// Scale
			bool updateScale = false;
			ImGui::PushID("Scale");
			if(ImGui::InputFloat("X", &transform->scale.x, step, stepFast))
				updateScale = true;
					
			ImGui::SameLine();
			if(ImGui::InputFloat("Y", &transform->scale.y, step, stepFast))
				updateScale = true;
					
			ImGui::SameLine();
			if(ImGui::InputFloat("Z", &transform->scale.z, step, stepFast))
				updateScale = true;

			if(updateScale)
				Transform::setScale(transform, transform->scale);

			ImGui::SameLine();
			ImGui::Text("Scale");
			ImGui::PopID();
			ImGui::PopItemWidth();
		}
	}

	void displayLight()
	{
		if(ImGui::CollapsingHeader("Light", "LightComponent", true, true))
		{
			ImGui::SliderFloat("Intensity", &light->intensity, 0.f, 10.f);
			ImGui::ColorEdit4("Color", glm::value_ptr(light->color));
			ImGui::Combo("Type", &light->type, "Spot\0Directional\0Point\0\0", 3);
			ImGui::Checkbox("Cast Shadows", &light->castShadow);
			if(light->type == LT_SPOT || light->type == LT_POINT)
			{
				if(light->type == LT_SPOT)
				{
					ImGui::SliderAngle("Inner", &light->innerAngle, 0, 180);
					ImGui::SliderAngle("Outer", &light->outerAngle, 0, 180);
				}
				if(ImGui::CollapsingHeader("Attenuation", "LC_Attenuation", false, false))
				{
					ImGui::InputFloat("Constant", &light->attenuation.constant, 0.1f, 0.25f);
					ImGui::InputFloat("Linear", &light->attenuation.linear, 0.01f, 0.1f);
					ImGui::InputFloat("Quadratic", &light->attenuation.quadratic, 0.0001f, 0.001f);
				}
			}
		}
	}

	void displayModel()
	{
		if(ImGui::CollapsingHeader("Model", "ModelComponent", true, true))
		{
			if(ImGui::InputText("Mesh", &inputModelName[0], BUF_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				CModel testModel;
				if(Renderer::Model::loadFromFile(&inputModelName[0], &testModel))
				{
					int prevMaterial = model->material;
					Mat_Uniforms prevUniforms = model->materialUniforms;
					const char* textureName =
						prevUniforms.texture != -1 ? Texture::getFilename(prevUniforms.texture) : NULL;
					int texture = textureName ? Texture::create(textureName) : -1;
					CModel* newModel = GO::addModel(selectedGO, &inputModelName[0]);
					if(!newModel)
					{
						memset(&inputModelName[0], '\0', BUF_SIZE);
						strcpy(&inputModelName[0], "NONE");
					}
					else
					{
						Renderer::Model::setMaterialType(newModel, (Mat_Type)prevMaterial);
						newModel->materialUniforms = prevUniforms;
						newModel->materialUniforms.texture = texture;
						updateComponentViewers();
						return;
					}
				}
				else
				{
					Log::error("Editor::displayModel",
							   "File " + std::string(&inputModelName[0]) + " not found, reverting back to previously attached");
					memset(&inputModelName[0], '\0', BUF_SIZE);
					int copySize = model->filename.size() > BUF_SIZE ? BUF_SIZE : model->filename.size();
					strncpy(&inputModelName[0], model->filename.c_str(), copySize);
				}
			}
			if(ImGui::IsItemHovered())
				ImGui::SetTooltip("Enter new name and press Enter to reload Model");

			int material = model->material;
			if(ImGui::Combo("Material Type",
							&material,
							"Unshaded\0Unshaded Textured\0Phong\0Phong Textured\0\0"))
			{				
				Renderer::Model::setMaterialType(model, (Mat_Type)material);
				memset(&inputModelTex[0], '\0', BUF_SIZE);
				// If model had texture before, copy its name otherwise copy NONE
				if(model->materialUniforms.texture != -1)
				{
					const char* texName = Texture::getFilename(model->materialUniforms.texture);
					size_t texNameLen = strlen(texName);
					int copySize = texNameLen > BUF_SIZE ? BUF_SIZE : texNameLen;
					strncpy(&inputModelTex[0], texName, copySize);
				}
				else
				{
					strcpy(&inputModelTex[0], "NONE");
				}
			}
			ImGui::ColorEdit4("Diffuse Color", &model->materialUniforms.diffuseColor[0]);
			// Texture
			if(model->material == MAT_PHONG_TEXTURED || model->material == MAT_UNSHADED_TEXTURED)
			{
				if(ImGui::InputText("Texture", &inputModelTex[0], BUF_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					int index = Texture::create(&inputModelTex[0]);
					if(index >= 0)
					{
						Texture::remove(model->materialUniforms.texture);
						model->materialUniforms.texture = index;
					}
					else
					{
						memset(&inputModelTex[0], '\0', BUF_SIZE);
						// If model had texture before, copy its name otherwise copy NONE
						if(model->materialUniforms.texture != -1)
						{
							const char* texName = Texture::getFilename(model->materialUniforms.texture);
							size_t texNameLen = strlen(texName);
							int copySize = texNameLen > BUF_SIZE ? BUF_SIZE : texNameLen;
							strncpy(&inputModelTex[0], texName, copySize);
						}
						else
						{
							strcpy(&inputModelTex[0], "NONE");
						}
					}
				}
			}
			if(model->material == MAT_PHONG || model->material == MAT_PHONG_TEXTURED)
			{
				ImGui::SliderFloat("Diffuse", &model->materialUniforms.diffuse, 0.f, 1.f);
				ImGui::SliderFloat("Specular", &model->materialUniforms.specular, 0.f, 1.f);
				ImGui::InputFloat("Specular Strength", &model->materialUniforms.specularStrength, 1.f, 2.f);
			}	
		}
	}

	void displayCamera()
	{
		if(ImGui::CollapsingHeader("Camera", "CameraComponent", true, true))
		{
			bool updateProj = false;
			if(ImGui::InputFloat("NearZ", &camera->nearZ, 0.1f, 1.f))
				updateProj = true;
			if(ImGui::InputFloat("FarZ", &camera->farZ, 1.f, 5.f))
				updateProj = true;
			if(ImGui::InputFloat("Fov", &camera->fov, 1.f, 5.f))
				updateProj = true;
			if(updateProj)
				Renderer::Camera::updateProjection(camera);
		}		
	}
	
	void update(float deltaTime, bool* quit)
	{
		ImGui::Begin("Tools", &showMainToolBar, Vec2(100, 20), OPACITY, (WF_NoTitleBar | WF_NoCollapse));

		//TODO : Use a union to switch between main windows related to the toolbar
		if(ImGui::Button("Log"))
			showLog = !showLog;
		ImGui::SameLine();
		
		if(ImGui::Button("Scene"))
			showSceneObjects = !showSceneObjects;
		ImGui::SameLine();

		if(ImGui::Button("Renderer"))
			showRendererSettings = !showRendererSettings;
		ImGui::SameLine();

		if(ImGui::Button("Scripting"))
			showScriptingWindow = !showScriptingWindow;
		ImGui::SameLine();

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
			ImGui::Text("Total : %d", gameObjects->size());
			if(ImGui::ListBox("GameObjects", &currentItem, &selectGameObject, gameObjects, gameObjects->size(), 10))
			{
				// Selected gameobject changed so update all pointers
				showSelectedGO = true;
				Node node = gameObjects->at(currentItem);
				selectedGO = SceneManager::find(node);
				memset(&inputName[0], '\0', BUF_SIZE);
				int copySize = selectedGO->name.size() > BUF_SIZE ? BUF_SIZE : selectedGO->name.size();
				memcpy(&inputName[0], &selectedGO->name[0], copySize);
				copySize = selectedGO->tag.size() > BUF_SIZE ? BUF_SIZE : selectedGO->tag.size();
				memcpy(&inputTag[0], &selectedGO->tag[0], copySize);
				updateComponentViewers();
			}
			
			if(ImGui::Button("Remove"))
			{
				if(showSelectedGO && selectedGO)
				{
					// Remove the gameobject and reset state
					SceneManager::remove(selectedGO->node);
					showSelectedGO = false;
					memset(&inputName[0], '\0', BUF_SIZE);
					memset(&inputNewName[0], '\0', BUF_SIZE);
					memset(&inputTag[0], '\0', BUF_SIZE);
					memset(&inputModelName[0], '\0', BUF_SIZE);
					memset(&inputModelTex[0], '\0', BUF_SIZE);
					transform = NULL;
					light     = NULL;
					model     = NULL;
					camera    = NULL;
				}
			}

			if(ImGui::InputText("Create New",
								&inputNewName[0],
								BUF_SIZE,
								ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if(strlen(&inputNewName[0]) > 0)
				{
					SceneManager::create(&inputNewName[0]);
					memset(&inputNewName[0], '\0', BUF_SIZE);
				}
				else
				{
					Log::error("Editor::addNewGameObject", "Name not entered, no gameobject created");
				}
			}
			if(ImGui::IsItemHovered())
				ImGui::SetTooltip("Insert name of new GameObject and press Enter to add it to the scene");
			ImGui::End();

			// Show currently selected gameobject, if any
			if(showSelectedGO)
			{
				ImGui::Begin(selectedGO->name.c_str(), &showSelectedGO, Vec2(450, 400), OPACITY);
				if(ImGui::InputText("Name", &inputName[0], BUF_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
					selectedGO->name = inputName;

				if(ImGui::InputText("Tag", &inputTag[0], BUF_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
					selectedGO->tag = inputTag;

				int selected = 0;
				const char* components = "None\0Camera\0Model\0Light\0Rigidbody\0\0";
				if(ImGui::Combo("Add New Component", &selected, components, 5))
				{
					CModel* newModel = NULL;
					switch(selected)
					{
					case 1:
						if(GO::hasComponent(selectedGO, Component::CAMERA))
						{
							Log::warning("Removing existing camera from " + selectedGO->name);
							GO::removeComponent(selectedGO, Component::CAMERA);
						}
						GO::addCamera(selectedGO);
						break;
					case 2:
						// if(GO::hasComponent(selectedGO, Component::MODEL))
						// {
						// 	Log::warning("Removing existing model from " + selectedGO->name);
						// 	GO::removeComponent(selectedGO, Component::MODEL);
						// }
						newModel = GO::addModel(selectedGO, "default.pamesh");
						Renderer::Model::setMaterialType(newModel, MAT_UNSHADED_TEXTURED);						
						newModel->materialUniforms.texture = Texture::create("default.png");
						newModel->materialUniforms.diffuseColor = Vec4(1.f, 0.f, 1.f, 1.f);						
						break;
					case 3:
						if(GO::hasComponent(selectedGO, Component::LIGHT))
						{
							Log::warning("Removing existing light from " + selectedGO->name);
							GO::removeComponent(selectedGO, Component::LIGHT);
						}
						GO::addLight(selectedGO);
						break;
					case 4:
						Log::warning("Not implemented!");
						break;
					default:
						break;
					}
					updateComponentViewers();
				}

				// Display Components
				// Transform Component
				displayTransform();
				// Light Component
				if(GO::hasComponent(selectedGO, Component::LIGHT))
					displayLight();
				// Model Component
				if(GO::hasComponent(selectedGO, Component::MODEL))
					displayModel();
				// Camera Component
				if(GO::hasComponent(selectedGO, Component::CAMERA))
					displayCamera();
				
				ImGui::End();
			}
		}
	}
	
	void cleanup()
	{
		
	}
}
