#include "gameobject.h"
#include "scriptengine.h"
#include "physics.h"
#include "transform.h"
#include "model.h"
#include "camera.h"
#include "light.h"

namespace GO
{
    bool hasComponent(GameObject* gameObject, Component type)
	{
		if(gameObject->compIndices[(int)type] == -1)
			return false;
		else
			return true;
	}
	
	void generateBindings()
	{
		Sqrat::RootTable().Bind("GameObject", Sqrat::Class<GameObject>()
								.Var("node",   &GameObject::node)
								.Var("name",   &GameObject::name)
								.Var("tag",    &GameObject::tag)
								.Var("remove", &GameObject::remove));

		Sqrat::ConstTable().Enum("Component", Sqrat::Enumeration()
								 .Const("TRANSFORM", Component::TRANSFORM)
								 .Const("CAMERA",    Component::CAMERA)
								 .Const("MODEL",     Component::MODEL)
								 .Const("RIGIDBODY", Component::RIGIDBODY)
								 .Const("LIGHT",     Component::LIGHT));

		Sqrat::RootTable().Bind("GO", Sqrat::Table (ScriptEngine::getVM())
								.Func("hasComponent", &hasComponent)
 								.Func("addTransform", &addTransform)
								.Func("addModel", &addModel)
								.Func("addLight", &addLight)
								// .Func("addRigidBody", &addRigidBody)
								.Func("getTransform", &getTransform)
								.Func("getCamera", &getCamera)
								.Func("getLight", &getLight)
								// .Func("getRigidBody", &getRigidBody)
								.Func("getModel", &getModel)
								.Func("removeComponent", &removeComponent));
	}

	void attachScript(GameObject* gameObject, const std::string& name)
	{
		ScriptEngine::executeFunction("attachScript", gameObject, name);
	}

	void reloadScript(GameObject* gameObject, const std::string& name)
	{
		ScriptEngine::executeFunction("reloadScript", gameObject, name);
	}

	void reloadScript(const std::string& goName, const std::string& name)
	{
		ScriptEngine::executeFunction("reloadScript", goName, name);
	}

	void reloadAllScripts(GameObject* gameObject)
	{
		ScriptEngine::executeFunction("reloadAllScripts", gameObject);
	}

	void reloadAllScripts(const std::string& goName)
	{
		ScriptEngine::executeFunction("reloadAllScripts", goName);
	}
	
	void removeScript(GameObject* gameObject, const std::string& name)
	{
		ScriptEngine::executeFunction("removeScript", gameObject, name);
	}

	void removeScript(const std::string& goName, const std::string& name)
	{
		ScriptEngine::executeFunction("removeScript", goName, name);
	}

	void reloadScriptType(const std::string& typeName)
	{
		ScriptEngine::executeFunction("reloadScriptType", typeName);
	}

	void processCollision(GameObject* gameObject, const CollisionData& collisionData)
	{
		ScriptEngine::executeFunction("processCollision", gameObject, collisionData);
	}

	CTransform* addTransform(GameObject* gameObject)
	{
		assert(gameObject);

		CTransform* newTransform = NULL;
		
		if(!hasComponent(gameObject, Component::TRANSFORM))
		{
			int index = Transform::create(gameObject->node);
			
			gameObject->compIndices[(int)Component::TRANSFORM] = index;
			Log::message("Transform added to " + gameObject->name);
			newTransform = Transform::getTransformAtIndex(index);
		}
		else
		{
			Log::warning("Transform couldnot be added to " + gameObject->name +
						 " because it already has one");
		}

		return newTransform;
	}

	CLight* addLight(GameObject* gameObject)
	{
		assert(gameObject);

		CLight* newLight = NULL;
		
		if(!hasComponent(gameObject, Component::LIGHT))
		{
			int index = Renderer::Light::create(gameObject->node);
			
			gameObject->compIndices[(int)Component::LIGHT] = index;
			Log::message("Light added to " + gameObject->name);
			newLight = Renderer::Light::getLightAtIndex(index);
		}
		else
		{
			Log::warning("Light couldnot be added to " + gameObject->name + " because it already has one");
		}

		return newLight;
	}


	CCamera* addCamera(GameObject* gameObject)
	{
		assert(gameObject);

		CCamera* newCamera = NULL;

		if(!hasComponent(gameObject, Component::CAMERA))
		{
			int index = Renderer::Camera::create(gameObject);

			gameObject->compIndices[(int)Component::CAMERA] = index;
			Log::message("Camera added to " + gameObject->name);
			newCamera = Renderer::Camera::getCameraAtIndex(index);
		}
		else
		{
			Log::warning("Camera couldnot be added to " + gameObject->name +
						 " because it already has one");
		}

		return newCamera;
	}

	CModel* addModel(GameObject* gameObject, CModel* model)
	{
		assert(gameObject);

		CModel* newModel = NULL;

		if(!hasComponent(gameObject, Component::MODEL))
		{
			int index = Renderer::Model::create(model);

			gameObject->compIndices[(int)Component::MODEL] = index;
			Log::message("Model added to " + gameObject->name);
			newModel = Renderer::Model::getModelAtIndex(index);
			newModel->node = gameObject->node;
		}
		else
		{
			Log::warning("Model couldnot be added to " + gameObject->name +
						 " because it already has one");
		}

		return newModel;		
	}

	CModel* getModel(GameObject* gameObject)
	{
		assert(gameObject);
		CModel* model = NULL;
		
		if(hasComponent(gameObject, Component::MODEL))
			model = Renderer::Model::getModelAtIndex(gameObject->compIndices[(Component::MODEL)]);
		else
			Log::error("GO::getModel", gameObject->name + " does not have model component");

		return model;
	}

	CCamera* getCamera(GameObject* gameObject)
	{
		assert(gameObject);
		CCamera* camera = NULL;
		
		if(hasComponent(gameObject, Component::CAMERA))
			camera = Renderer::Camera::getCameraAtIndex(gameObject->compIndices[(Component::CAMERA)]);
		else
			Log::error("GO::getCamera", gameObject->name + " does not have camera component");

		return camera;
	}


	CTransform* getTransform(GameObject* gameObject)
	{
		assert(gameObject);
		CTransform* transform = NULL;
		
		if(hasComponent(gameObject, Component::TRANSFORM))
			transform = Transform::getTransformAtIndex(gameObject->compIndices[(Component::TRANSFORM)]);
		else
			Log::error("GO::getTransform", gameObject->name + " does not have transform component");

		return transform;
	}

	CLight* getLight(GameObject* gameObject)
	{
		assert(gameObject);
		CLight* light = NULL;
		
		if(hasComponent(gameObject, Component::LIGHT))
			light =Renderer::Light::getLightAtIndex(gameObject->compIndices[(Component::LIGHT)]);
		else
			Log::error("GO::getLight", gameObject->name + " does not have light component");

		return light;
	}


	void removeComponent(GameObject* gameObject, Component type)
	{
	    assert(gameObject);

		if(hasComponent(gameObject, type))
		{
			int index = gameObject->compIndices[(int)type];
			gameObject->compIndices[(int)type] = EMPTY_INDEX;
			
			switch(type)
			{
			case Component::TRANSFORM:
				Transform::remove(index);
				break;
					
			case Component::CAMERA:
				Renderer::Camera::remove(index);
				break;

			case Component::MODEL:
				Renderer::Model::remove(index);
				break;

			case Component::LIGHT:
				Renderer::Light::remove(index);
				break;
					
			case Component::RIGIDBODY:
				// Physics::RigidBody::remove(sRigidBodyList[index]);
				// sRigidBodyList[index] = -1;
				// sRigidBodyEmptyList.push_back(index);
				break;
			case Component::NUM_COMPONENTS:
				Log::error("GO::removeComponent", "Cannot remove invalid component type");
				break;
			case Component::EMPTY:
				Log::error("GO::removeComponent", "Cannot remove invalid component type");
				break;
			}
		}
	}
}
