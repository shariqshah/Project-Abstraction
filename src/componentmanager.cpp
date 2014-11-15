#include "componentmanager.h"
#include "motionstate.h"
#include "gameobject.h"
#include "camera.h"
#include "model.h"
#include "light.h"
#include "physics.h"
#include "transform.h"
#include "renderer.h"
#include "scriptengine.h"

namespace CompManager
{
	namespace
	{
		static std::vector<CTransform> sTransformList;
		static std::vector<int>        sTransformEmptyList;

		static std::vector<CCamera>    sCameraList;
		static std::vector<int>        sCameraEmptyList;

		static std::vector<CModel>     sModelList;
		static std::vector<int>        sModelEmptyList;

		static std::vector<CLight>     sLightList;
		static std::vector<int>        sLightEmptyList;

		static std::vector<CRigidBody> sRigidBodyList;
		static std::vector<int>        sRigidBodyEmptyList;
	}

	void initailize()
	{
		
	}

	CTransform* getTransform(GameObject* gameObject)
	{
		if(gameObject)
			return &sTransformList[gameObject->compIndices[(int)Component::TRANSFORM]];
		else
			return NULL;
	}

	CCamera* getCamera(GameObject* gameObject)
	{
		if(gameObject)
			return &sCameraList[gameObject->compIndices[(int)Component::CAMERA]];
		else
			return NULL;
	}

	CModel* getModel(GameObject* gameObject)
	{
		if(gameObject)
			return &sModelList[gameObject->compIndices[(int)Component::MODEL]];
		else
			return NULL;
	}

	CLight* getLight(GameObject* gameObject)
	{
		if(gameObject)
			return &sLightList[gameObject->compIndices[(int)Component::LIGHT]];
		else
			return NULL;
	}

	CRigidBody getRigidBody(GameObject* gameObject)
	{
		if(gameObject)
			return sRigidBodyList[gameObject->compIndices[(int)Component::RIGIDBODY]];
		else
			return 0;
	}

	CRigidBody addRigidBody(GameObject* gameObject,
							CollisionShape* shape,
							float mass,
							float restitution)
	{
		btMotionState* motionState = new MotionState(gameObject);
		CRigidBody newRigidBody  = Physics::RigidBody::create(shape,
															  motionState,
															  mass,
															  restitution);
		int index = 0;
			
		if(sRigidBodyEmptyList.empty())
		{
			sRigidBodyList.push_back(newRigidBody);
			index = sRigidBodyList.size() - 1;
		}
		else
		{
			index = sRigidBodyEmptyList.back();
			sRigidBodyList[index] = newRigidBody;
			sRigidBodyEmptyList.pop_back();
		}

		gameObject->compIndices[(int)Component::RIGIDBODY] = index;
		Log::message("RigidBody added to " + gameObject->name);

		return sRigidBodyList[index];
	}

	CLight* addLight(GameObject* gameObject, const std::string& name)
	{
		CLight newLight = Renderer::Light::create(gameObject->node, name);

		if(newLight.node == 0)
		{
			Log::error("CompManager", "Light could not be added to "
					   + gameObject->name +	". Check renderer logs.");
			return NULL;
		}
		else
		{
			int index = 0;
			
			if(sLightEmptyList.empty())
			{
				sLightList.push_back(newLight);
				index = sLightList.size() - 1;
			}
			else
			{
				index = sLightEmptyList.back();
				sLightList[index] = newLight;
				sLightEmptyList.pop_back();
			}

			gameObject->compIndices[(int)Component::LIGHT] = index;
			Log::message("Light added to " + gameObject->name);
			return &sLightList[index];
		}
	}
	
	CModel* addModel(GameObject* gameObject, const std::string& filename)
	{
		CModel newModel;

		newModel.filename   = filename;
		newModel.resourceID = Renderer::Resources::get(ResourceType::MODEL,
													   newModel.filename);

		if(newModel.resourceID == 0)
		{
			Log::error("CompManager", "Model could not be added to gameobject. Check renderer logs.");
			return NULL;
		}
		else
		{
			newModel.node = Renderer::createNode(newModel.resourceID,
												 gameObject->node);
			if(newModel.node == 0)
			{
				Log::error("CompManager", "Model node could not be parented to GameObject. Check renderer logs.");
				return NULL;
			}
			else
			{
				int index = 0;
			
				if(sModelEmptyList.empty())
				{
					sModelList.push_back(newModel);
					index = sModelList.size() - 1;
				}
				else
				{
					index = sModelEmptyList.back();
					sModelList[index] = newModel;
					sModelEmptyList.pop_back();
				}

				gameObject->compIndices[(int)Component::MODEL] = index;
				Log::message("Model added to " + gameObject->name);
				return &sModelList[index];
			}
				
		}
	}

	CCamera* addCamera(GameObject* gameObject, const std::string& name)
	{
		if(gameObject)
		{
			if(!GO::hasComponent(gameObject, Component::CAMERA))
			{
				CCamera newCamera = Renderer::Camera::create(name, gameObject->node);

				if(newCamera.valid)
				{
					int index = 0;
			
					if(sCameraEmptyList.empty())
					{
						sCameraList.push_back(newCamera);
						index = sCameraList.size() - 1;
					}
					else
					{
						index = sCameraEmptyList.back();
						sCameraList[index] = newCamera;
						sCameraEmptyList.pop_back();
					}

					gameObject->compIndices[(int)Component::CAMERA] = index;
					Log::message("Camera added to " + gameObject->name);
					return &sCameraList[index];
				}
				else
				{
					Log::error("CompManager", "Camera could not be added to "
							   + gameObject->name + " because of some renderer error. Check logs.");
					return NULL;
				}
			}
			else
			{
				Log::warning("Camera couldnot be added to " + gameObject->name +
							 " because it already has one");
				return NULL;
			}
		}
		else
		{
			Log::error("CompManager", "Camera could not be added to gameobject because it is NULL");
			return NULL;
		}
	}

	CTransform* addTransform(GameObject* gameObject)
	{
		if(gameObject)
		{
			if(!GO::hasComponent(gameObject, Component::TRANSFORM))
			{
			    CTransform newTransform;
				newTransform.node = gameObject->node;
				Transform::updateTransformMatrix(&newTransform);
				int index = 0;
			
				if(sTransformEmptyList.empty())
				{
					sTransformList.push_back(newTransform);
					index = sTransformList.size() - 1;
				}
				else
				{
					index = sTransformEmptyList.back();
					sTransformList[index] = newTransform;
					sTransformEmptyList.pop_back();
				}

				gameObject->compIndices[(int)Component::TRANSFORM] = index;
				Log::message("Transform added to " + gameObject->name);
				return &sTransformList[index];
			}
			else
			{
				Log::warning("Transform couldnot be added to " + gameObject->name +
							 " because it already has one");
				return NULL;
			}
		}
		else
		{
			Log::error("CompManager", "Transform could not be added to gameobject because it is NULL");
			return NULL;
		} 
		
	}

	void removeComponent(GameObject* gameObject, Component type)
	{
		if(gameObject)
		{
			if(GO::hasComponent(gameObject, type))
			{
				int index = -1;
				switch(type)
				{
				case Component::TRANSFORM:
					index = gameObject->compIndices[(int)type];
					gameObject->compIndices[(int)type] = EMPTY_INDEX;
					sTransformList[index].valid = false;
					sTransformEmptyList.push_back(index);
					break;
					
				case Component::CAMERA:
					index = gameObject->compIndices[(int)type];
					gameObject->compIndices[(int)type] = EMPTY_INDEX;
					Renderer::Camera::removeCamera(sCameraList[index]);
					sCameraList[index].valid = false;
					sCameraEmptyList.push_back(index);
					break;

				case Component::MODEL:
					index = gameObject->compIndices[(int)type];
					gameObject->compIndices[(int)type] = EMPTY_INDEX;
					Renderer::Model::remove(sModelList[index]);
					sModelList[index].valid = false;
					sModelEmptyList.push_back(index);
					break;

				case Component::LIGHT:
					index = gameObject->compIndices[(int)type];
					gameObject->compIndices[(int)type] = EMPTY_INDEX;
					Renderer::removeNode(sLightList[index].node);
					sLightList[index].valid = false;
					sLightEmptyList.push_back(index);
					break;
					
				case Component::RIGIDBODY:
					index = gameObject->compIndices[(int)type];
					gameObject->compIndices[(int)type] = EMPTY_INDEX;
					Physics::RigidBody::remove(sRigidBodyList[index]);
					sRigidBodyList[index] = -1;
					sRigidBodyEmptyList.push_back(index);
					break;
				case Component::NUM_COMPONENTS:
					Log::error("CompManager", "Cannot remove invalid component type");
					break;
				case Component::EMPTY:
					Log::error("CompManager", "Cannot remove invalid component type");
					break;
				}
			}
		}
	}

	void generateBindings()
	{
		Sqrat::ConstTable().Enum("Component", Sqrat::Enumeration()
								 .Const("TRANSFORM", Component::TRANSFORM)
								 .Const("CAMERA",    Component::CAMERA)
								 .Const("MODEL",     Component::MODEL)
								 .Const("RIGIDBODY", Component::RIGIDBODY)
								 .Const("LIGHT",     Component::LIGHT));

		Sqrat::RootTable().Bind("CompManager", Sqrat::Table(ScriptEngine::getVM())
								.Func("addTransform", &addTransform)
								.Func("addModel", &addModel)
								.Func("addLight", &addLight)
								.Func("addRigidBody", &addRigidBody)
								.Func("getTransform", &getTransform)
								.Func("getCamera", &getCamera)
								.Func("getLight", &getLight)
								.Func("getRigidBody", &getRigidBody)
								.Func("getModel", &getModel)
								.Func("removeComponent", &removeComponent));
	}

	void cleanup()
	{
		
	}
}
