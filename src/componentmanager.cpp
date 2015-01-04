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
		std::vector<CTransform> sTransformList;
		std::vector<int>        sTransformEmptyList;

		std::vector<CCamera>    sCameraList;
		std::vector<int>        sCameraEmptyList;

		std::vector<CModel>     sModelList;
		std::vector<int>        sModelEmptyList;

		std::vector<CLight>     sLightList;
		std::vector<int>        sLightEmptyList;

		std::vector<CRigidBody> sRigidBodyList;
		std::vector<int>        sRigidBodyEmptyList;
	}

    void initialize()
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
		CRigidBody newRigidBody  = Physics::RigidBody::create(gameObject,
															  shape,
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

	}

	CCamera* addCamera(GameObject* gameObject, const std::string& name, int pipeline)
	{

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
					Renderer::Camera::remove(index);
					break;

				case Component::MODEL:
					index = gameObject->compIndices[(int)type];
					gameObject->compIndices[(int)type] = EMPTY_INDEX;
					Renderer::Model::remove(index);
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
