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
#include "renderer.h"
#include "passert.h"
#include "physics.h"
#include "console.h"
#include "collisionshapes.h"
#include "rigidbody.h"
#include "geometry.h"
#include "utilities.h"

namespace Editor
{
	struct DebugInt
	{
		const char* description;
		int         value;
		DebugInt(const char* desc, int val) : description(desc), value(val) {}
	};

	struct DebugFloat
	{
		const char* description;
		float         value;
		DebugFloat(const char* desc, float val) : description(desc), value(val) {}
	};
	
	namespace
	{
		bool showMainToolBar      = true;
		bool showLog              = false;
		bool showSceneObjects     = false;
		bool showSelectedGO       = false;
		bool showRendererSettings = false;
		bool showScriptingWindow  = false;
		bool showStatsWindow      = true;
		bool showPhysicsWindow    = false;
		bool showSample           = false;
		bool showDebugVars        = false;

		int  currentItem = 1;
		
		const float OPACITY  = 1.f;
		const int   BUF_SIZE = 128;

		char inputName[BUF_SIZE]       = "";
		char inputNewName[BUF_SIZE]    = "";
		char inputTag[BUF_SIZE]        = "";
		char inputGeoName[BUF_SIZE]    = "";
		char inputModelTex[BUF_SIZE]   = "";
		Node selectedGONode = -1;

		float updateTime = 0.f;
		float drawTime   = 0.f;

		int   collisionShapeCombo = 0;
		bool  showChangeCollisionShapeMenu = false;
		float csRadius;
		float csHeight;
		float csMargin;
		Vec3  csHalfExt;
		Vec3  csNormal;
		Vec3  csAxis;
		bool  csIsTriMesh;
		char  csInputMeshName[BUF_SIZE];

		ImGuiTextFilter gameObjectFilter;
		std::vector<DebugInt> debugInts;
		std::vector<DebugFloat> debugFloats;
	}
	
	void initialize()
	{
	}

	void setUpdateTime(const float time)
	{
		updateTime = time;
	}

	void setDrawTime(const float time)
	{
		drawTime = time;
	}
	
	void resetCollisionShapeParams()
	{
		// showChangeCollisionShapeMenu = false;
		csRadius    = 0.f;
		csHeight    = 0.f;
		csMargin    = 0.f;
		csHalfExt   = Vec3(0.f);
		csNormal    = Vec3(0, 1, 0);
		csAxis      = Vec3(0, 1, 0);
		csIsTriMesh = false;
		memset(&csInputMeshName[0], '\0', BUF_SIZE);
	}

	void updateComponentViewers()
	{
		GameObject* selectedGO = SceneManager::find(selectedGONode);
		// Check Model
		if(GO::hasComponent(selectedGO, Component::MODEL))
		{
			CModel *model = GO::getModel(selectedGO);
			memset(&inputGeoName[0], '\0', BUF_SIZE);
			memset(&inputModelTex[0], '\0', BUF_SIZE);
			const std::string filename = Geometry::getName(model->geometryIndex);
			size_t copySize = filename.size() > BUF_SIZE ? BUF_SIZE : filename.size();
			strncpy(&inputGeoName[0], filename.c_str(), copySize);
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
		resetCollisionShapeParams();
		showChangeCollisionShapeMenu = false;
		collisionShapeCombo = 0;
	}

	void displayRigidBody()
	{
		GameObject* selectedGO = SceneManager::find(selectedGONode);
		CRigidBody  body       = GO::getRigidBody(selectedGO);
		if(ImGui::CollapsingHeader("RigidBody", "RigidBodyComponent", true, true))
		{
			float mass = RigidBody::getMass(body);
			if(ImGui::InputFloat("Mass", &mass, 1.f, 5.f))
				RigidBody::setMass(body, mass);

			float restitution = RigidBody::getRestitution(body);
			if(ImGui::InputFloat("Restitution", &restitution, 0.01f, 0.1f))
				RigidBody::setRestitution(body, restitution);

			float friction = RigidBody::getFriction(body);
			if(ImGui::InputFloat("Friction", &friction, 0.1f, 0.5f))
				RigidBody::setFriction(body, friction);
			
			ImGui::Text("Collision Shape : %s ", RigidBody::getCollisionShapeName(body));
			if(ImGui::Combo("Set collision shape",
							&collisionShapeCombo,
							"None\0Box\0Sphere\0Capsule\0Cylinder\0Plane\0Mesh\0\0"))
			{
				if(collisionShapeCombo > 0)
				{
					showChangeCollisionShapeMenu = true;
					resetCollisionShapeParams();
				}
				else
				{
					showChangeCollisionShapeMenu = false;
				}
			}

			if(showChangeCollisionShapeMenu)
			{
				CModel* model = NULL;
				switch(collisionShapeCombo)
				{
				case 1:
					ImGui::InputFloat3("Half Extents", &csHalfExt[0]);
					break;
				case 2:
					ImGui::InputFloat("Radius", &csRadius, 1.f, 5.f);
					break;
				case 3:
					ImGui::InputFloat("Radius", &csRadius, 1.f, 5.f);
					ImGui::InputFloat("Height", &csHeight, 1.f, 5.f);
					break;
				case 4:
					ImGui::InputFloat3("Half Extents", &csHalfExt[0]);
					ImGui::InputFloat3("Axis", &csAxis[0]);
					break;
				case 5:
					ImGui::InputFloat3("Normal", &csNormal[0]);
					ImGui::InputFloat("Margin", &csMargin, 0.01f, 0.1f);
					break;
				case 6:
					ImGui::InputText("CollisionMesh Name", &csInputMeshName[0], BUF_SIZE);
					ImGui::Checkbox("Is TriMesh", &csIsTriMesh);
					break;
				}

				if(ImGui::Button("Change Collision Shape"))
				{
					switch(collisionShapeCombo)
					{
					case 1: RigidBody::setCollisionShape(body, new Box(csHalfExt));   break;
					case 2:	RigidBody::setCollisionShape(body, new Sphere(csRadius)); break;
					case 3: RigidBody::setCollisionShape(body, new Capsule(csRadius, csHeight)); break;
					case 4: RigidBody::setCollisionShape(body, new Cylinder(csHalfExt, csAxis)); break;
					case 5: RigidBody::setCollisionShape(body, new Plane(csNormal, csMargin)); break;
					case 6:
						RigidBody::setCollisionShape(body, new CollisionMesh(&csInputMeshName[0], csIsTriMesh));
						break;
					}
					resetCollisionShapeParams();
					showChangeCollisionShapeMenu = false;
					collisionShapeCombo = 0;
				}
				ImGui::SameLine();
				if(ImGui::Button("Cancel"))
				{
					resetCollisionShapeParams();
					showChangeCollisionShapeMenu = false;
					collisionShapeCombo = 0;
				}
			}
		}
	}
	
	void displayTransform()
	{
		GameObject* selectedGO = SceneManager::find(selectedGONode);
		CTransform* transform  = GO::getTransform(selectedGO);
		if(ImGui::CollapsingHeader("Transform", "TransformComponent", true, true))
		{
			float step     = 1.f;
			float stepFast = 10.f;
			float width    = ImGui::GetWindowWidth() / 5.f;
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
		GameObject* selectedGO = SceneManager::find(selectedGONode);
		CLight*     light      = GO::getLight(selectedGO);
		if(ImGui::CollapsingHeader("Light", "LightComponent", true, true))
		{
			ImGui::SliderFloat("Intensity", &light->intensity, 0.f, 10.f);
			ImGui::ColorEdit4("Color", glm::value_ptr(light->color));
			ImGui::Combo("Type", &light->type, "Spot\0Directional\0Point\0\0", 3);
			ImGui::Checkbox("Cast Shadows", &light->castShadow);
			if(light->type == LT_SPOT || light->type == LT_POINT)
			{
				ImGui::InputInt("Radius", &light->radius, 1.f, 5.f);
				ImGui::InputFloat("Falloff", &light->falloff, 0.5f, 5.f);
				if(light->type == LT_SPOT)
				{
					ImGui::SliderAngle("Inner", &light->innerAngle, 0, 180);
					ImGui::SliderAngle("Outer", &light->outerAngle, 0, 180);
				}
			}
		}
	}

	void displayModel()
	{
		GameObject* selectedGO = SceneManager::find(selectedGONode);
		CModel*     model      = GO::getModel(selectedGO);
		if(ImGui::CollapsingHeader("Model", "ModelComponent", true, true))
		{
			if(ImGui::InputText("Geometry", &inputGeoName[0], BUF_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if(Utils::fileExists(&inputGeoName[0]))
				{
					int prevMaterial = model->material;
					Mat_Uniforms prevUniforms = model->materialUniforms;
					const char* textureName =
						prevUniforms.texture != -1 ? Texture::getFilename(prevUniforms.texture) : NULL;
					int texture = textureName ? Texture::create(textureName) : -1;
					CModel* newModel = GO::addModel(selectedGO, &inputGeoName[0]);
					if(!newModel)
					{
						memset(&inputGeoName[0], '\0', BUF_SIZE);
						strcpy(&inputGeoName[0], "NONE");
					}
					else
					{
						Model::setMaterialType(newModel, (Mat_Type)prevMaterial);
						newModel->materialUniforms = prevUniforms;
						newModel->materialUniforms.texture = texture;
						updateComponentViewers();
						return;
					}
				}
				else
				{
					Log::error("Editor::displayModel",
							   "File " + std::string(&inputGeoName[0]) + " not found, reverting back to previously attached");
					memset(&inputGeoName[0], '\0', BUF_SIZE);
					const std::string filename = Geometry::getName(model->geometryIndex);
					int copySize = filename.size() > BUF_SIZE ? BUF_SIZE : filename.size();
					strncpy(&inputGeoName[0], filename.c_str(), copySize);
				}
			}
			if(ImGui::IsItemHovered())
				ImGui::SetTooltip("Insert new name and press Enter to reload Model");

			int material = model->material;
			if(ImGui::Combo("Material Type",
							&material,
							"Unshaded\0Unshaded Textured\0Phong\0Phong Textured\0\0"))
			{				
				Model::setMaterialType(model, (Mat_Type)material);
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
		GameObject* selectedGO = SceneManager::find(selectedGONode);
		CCamera*    camera     = GO::getCamera(selectedGO);
		if(ImGui::CollapsingHeader("Camera", "CameraComponent", true, true))
		{
			bool updateProj = false;
			if(ImGui::InputFloat("NearZ", &camera->nearZ, 0.1f, 1.f)) updateProj = true;
			if(ImGui::InputFloat("FarZ", &camera->farZ, 1.f, 5.f))    updateProj = true;
			if(ImGui::InputFloat("Fov", &camera->fov, 1.f, 5.f))	  updateProj = true;
			if(updateProj) Camera::updateProjection(camera);

			CCamera* activeCamera = Camera::getActiveCamera();
			bool isActive = false;
			if(activeCamera) isActive = camera->node == activeCamera->node ? true : false;
			if(ImGui::Checkbox("Is Active", &isActive))
			{
				if(isActive) Camera::setActiveCamera(camera);
				else		 Camera::setActiveCamera(NULL);
			}
			if(ImGui::IsItemHovered())
				ImGui::SetTooltip("Check to make the camera the active viewer for the scene");
		}
	}

	void displaySceneObjects()
    {
		ImGui::Begin("SceneObjects", &showSceneObjects, Vec2(400, 350), OPACITY);
		
		std::vector<Node>* gameObjectNodes = SceneManager::getSceneObjects();
		ImGui::Text("Total : %d", gameObjectNodes->size());
		gameObjectFilter.Draw("Filter by name");

		if(ImGui::ListBoxHeader("GameObjects"))
		{
			for(uint32_t i = 0; i < gameObjectNodes->size(); i++)
			{
				GameObject* gameObject = SceneManager::find(gameObjectNodes->at(i));
				if(gameObject && gameObjectFilter.PassFilter(gameObject->name.c_str()))
				{
					bool selected = selectedGONode == gameObject->node ? true : false;
					if(ImGui::Selectable(gameObject->name.c_str(), selected))
					{
						// Selected gameobject changed so update all pointers
						showSelectedGO = true;
						selectedGONode = gameObject->node;
						memset(&inputName[0], '\0', BUF_SIZE);
						int copySize = gameObject->name.size() > BUF_SIZE ? BUF_SIZE : gameObject->name.size();
						memcpy(&inputName[0], &gameObject->name[0], copySize);
						copySize = gameObject->tag.size() > BUF_SIZE ? BUF_SIZE : gameObject->tag.size();
						memcpy(&inputTag[0], &gameObject->tag[0], copySize);
						updateComponentViewers();
					}
				}
			}
		}
		ImGui::ListBoxFooter();
			
		if(ImGui::Button("Remove"))
		{
			if(showSelectedGO && selectedGONode != -1)
			{
				// Remove the gameobject and reset state
				SceneManager::remove(selectedGONode);
				showSelectedGO = false;
				memset(&inputName[0], '\0', BUF_SIZE);
				memset(&inputNewName[0], '\0', BUF_SIZE);
				memset(&inputTag[0], '\0', BUF_SIZE);
				memset(&inputGeoName[0], '\0', BUF_SIZE);
				memset(&inputModelTex[0], '\0', BUF_SIZE);
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
			GameObject* selectedGO = SceneManager::find(selectedGONode);
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
					newModel = GO::addModel(selectedGO, "default.pamesh");
					Model::setMaterialType(newModel, MAT_UNSHADED_TEXTURED);
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
					if(GO::hasComponent(selectedGO, Component::RIGIDBODY))
					{
						Log::warning("Removing existing rigidbody from " + selectedGO->name);
						GO::removeComponent(selectedGO, Component::RIGIDBODY);
					}
					GO::addRigidbody(selectedGO, new Box(Vec3(0.5f)));
					break;
				default:
					break;
				}
				updateComponentViewers();
			}
			if(ImGui::IsItemHovered())
				ImGui::SetTooltip("Components attached will be removed and attached again with defaults");

			// Remove component
			selected  = 0;
			if(ImGui::Combo("Remove Component", &selected, components, 5))
			{
				Component componentToRemove = Component::EMPTY;
				switch(selected)
				{
				case 1:	componentToRemove = Component::CAMERA;    break;
				case 2: componentToRemove = Component::MODEL;     break;
				case 3:	componentToRemove = Component::LIGHT;     break;
				case 4:	componentToRemove = Component::RIGIDBODY; break;
				default: break;
				}
				GO::removeComponent(selectedGO, componentToRemove);
				updateComponentViewers();
			}
			if(ImGui::IsItemHovered())
				ImGui::SetTooltip("Only components attached can be removed");

			// Display Components
			displayTransform();
			if(GO::hasComponent(selectedGO, Component::LIGHT))	   displayLight();
			if(GO::hasComponent(selectedGO, Component::MODEL))     displayModel();
			if(GO::hasComponent(selectedGO, Component::CAMERA))	   displayCamera();				
			if(GO::hasComponent(selectedGO, Component::RIGIDBODY)) displayRigidBody();				
			ImGui::End();
		}
	}

	void displayRendererSettings()
	{
		ImGui::Begin("Renderer Settings", &showRendererSettings, Vec2(400, 200), OPACITY);
		Vec4          clearColor    = Renderer::getClearColor();
		RenderParams* renderParams  = Renderer::getRenderParams();
		const char*   fogModeString = "None\0Linear\0Exponential\0Exponential Squared\0\0";
		
		if(ImGui::ColorEdit4("Clear Color", glm::value_ptr(clearColor), true))
			Renderer::setClearColor(clearColor);
		ImGui::ColorEdit4("Ambient Light", glm::value_ptr(renderParams->ambientLight), true);
		
		if(ImGui::CollapsingHeader("Fog", "RS_FOG", false, true))
		{
			ImGui::Combo("Mode", &renderParams->fog.fogMode, fogModeString, 4);
			if(renderParams->fog.fogMode != FG_NONE)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(renderParams->fog.color), true);
				ImGui::InputFloat("Density", &renderParams->fog.density, 0.001f, 0.01f);
				if(renderParams->fog.fogMode == FG_LINEAR)
				{
					ImGui::InputFloat("Start",   &renderParams->fog.start, 1.f, 5.f);
					ImGui::InputFloat("Max",     &renderParams->fog.max, 1.f, 5.f);
				}
			}
		}
		ImGui::End();		
	}

	void displayStatsWindow()
	{
		ImGui::Begin("Stats", &showStatsWindow, Vec2(10, 20), OPACITY, (WF_NoTitleBar | WF_NoCollapse));
		ImGui::Text("Fps    : %.1f", ImGui::GetIO().Framerate);
		ImGui::Text("Update : %.2f", updateTime);
		ImGui::Text("Draw   : %.2f", drawTime);
		ImGui::End();
	}

	void displayPhysicsWindow()
	{
		ImGui::Begin("Physics", &showPhysicsWindow, Vec2(80, 20), OPACITY, WF_NoCollapse);
		bool physicsEnabled = Physics::isEnabled();
		bool debugEnabled   = Physics::isDebugDrawerEnabled();
		Vec3 gravity        = Physics::getGravity();
		
		if(ImGui::Checkbox("Physics Enabled", &physicsEnabled))	Physics::enable(physicsEnabled);
		if(ImGui::Checkbox("Debug Drawer", &debugEnabled))      Physics::enableDebugDraw(debugEnabled);
		if(ImGui::InputFloat3("Gravity", &gravity[0]))			Physics::setGravity(gravity);

		ImGui::End();
	}

	void displayDebugVars()
	{
		ImGui::Begin("Debug Variables", &showDebugVars, Vec2(40, 80), OPACITY, WF_NoCollapse);
		ImGui::Columns(2, "DebugInts");
		ImGui::Text("Description"); ImGui::NextColumn();
		ImGui::Text("Value");       ImGui::NextColumn();
		ImGui::Separator();
		for(DebugInt& debugInt : debugInts)
		{
			ImGui::Text(debugInt.description); ImGui::NextColumn();
			ImGui::Text("%d", debugInt.value); ImGui::NextColumn();
		}
		for(DebugFloat& debugFloat : debugFloats)
		{
			ImGui::Text(debugFloat.description);   ImGui::NextColumn();
			ImGui::Text("%1.f", debugFloat.value); ImGui::NextColumn();
		}
		ImGui::End();
	}

	void addDebugFloat(const char* description, float value)
	{
		debugFloats.push_back(DebugFloat(description, value));
	}
	
	void addDebugInt(const char* description, int value)
	{
		debugInts.push_back(DebugInt(description, value));
	}
	
	void update(float deltaTime, bool* quit)
	{
		ImGui::Begin("Tools", &showMainToolBar, Vec2(80, 20), OPACITY, (WF_NoTitleBar | WF_NoCollapse));
		if(ImGui::Button("Scene"))     showSceneObjects     = !showSceneObjects;     ImGui::SameLine();
		if(ImGui::Button("Renderer"))  showRendererSettings = !showRendererSettings; ImGui::SameLine();
		if(ImGui::Button("Physics"))   showPhysicsWindow    = !showPhysicsWindow;    ImGui::SameLine();
		if(ImGui::Button("Scripting")) showScriptingWindow  = !showScriptingWindow;  ImGui::SameLine();
		if(ImGui::Button("Sample"))    showSample           = !showSample;           ImGui::SameLine();
		if(ImGui::Button("DebugVars")) showDebugVars        = !showDebugVars;        ImGui::SameLine();
		if(ImGui::Button("Log"))
		{
			showLog = !showLog;
			Console::showWindow(showLog);
		}
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
		if(showSceneObjects)     displaySceneObjects();
		if(showRendererSettings) displayRendererSettings();
		if(showStatsWindow)      displayStatsWindow();
		if(showPhysicsWindow)    displayPhysicsWindow();
		if(showDebugVars)        displayDebugVars();
		if(showSample)           ImGui::ShowTestWindow(&showSample);

		debugInts.clear();
		debugFloats.clear();
	}
	
	void cleanup()
	{
		
	}
}
