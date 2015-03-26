#include "gameobject.h"
#include "scriptengine.h"
#include "physics.h"
#include "transform.h"
#include "model.h"
#include "camera.h"
#include "light.h"
#include "passert.h"

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
		asIScriptEngine* engine = ScriptEngine::getEngine();
		int rc = -1;
		engine->RegisterEnum("Component");
		rc = engine->RegisterEnumValue("Component", "TRANSFORM", (int)Component::TRANSFORM);
		rc = engine->RegisterEnumValue("Component", "CAMERA",    (int)Component::CAMERA);
		rc = engine->RegisterEnumValue("Component", "MODEL",     (int)Component::MODEL);
		rc = engine->RegisterEnumValue("Component", "LIGHT",     (int)Component::RIGIDBODY);
		rc = engine->RegisterEnumValue("Component", "RIGIDBODY", (int)Component::LIGHT);
		
		rc = engine->RegisterObjectType("GameObject", sizeof(GameObject), asOBJ_REF | asOBJ_NOCOUNT);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("GameObject", "string name", asOFFSET(GameObject, name));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("GameObject", "string tag", asOFFSET(GameObject, tag));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("GameObject", "int32 node", asOFFSET(GameObject, node));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("GameObject",
										  "Transform@ getTransform()",
										  asFUNCTION(getTransform),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("GameObject",
										  "Camera@ getCamera()",
										  asFUNCTION(getCamera),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("GameObject",
										  "Camera@ addCamera()",
										  asFUNCTION(addCamera),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("GameObject",
										  "Light@ getLight()",
										  asFUNCTION(getLight),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("GameObject",
										  "Light@ addLight(Vec4 = Vec4(1.0f))",
										  asFUNCTION(addLight),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("GameObject",
										  "Model@ getModel()",
										  asFUNCTION(getModel),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("GameObject",
										  "Model@ addModel(string)",
										  asFUNCTION(addModel),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("GameObject",
										  "void removeComponent(Component)",
										  asFUNCTION(removeComponent),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("GameObject",
										  "bool hasComponent(Component)",
										  asFUNCTION(hasComponent),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		
	}

	// void processCollision(GameObject* gameObject, const CollisionData& collisionData)
	// {
	// 	ScriptEngine::executeFunction("processCollision", gameObject, collisionData);
	// }

	CTransform* addTransform(GameObject* gameObject)
	{
		PA_ASSERT(gameObject);

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
			Log::warning("Transform not be added to " + gameObject->name +" because it already has one");
		}

		return newTransform;
	}

	CLight* addLight(GameObject* gameObject, Vec4 color)
	{
		PA_ASSERT(gameObject);
		CLight* newLight = NULL;
		if(!hasComponent(gameObject, Component::LIGHT))
		{
			int index = Renderer::Light::create(gameObject->node);
			
			gameObject->compIndices[(int)Component::LIGHT] = index;
			Log::message("Light added to " + gameObject->name);
			newLight = Renderer::Light::getLightAtIndex(index);
			newLight->color = color;
		}
		else
		{
			Log::warning("Light couldnot be added to " + gameObject->name + " because it already has one");
		}
		return newLight;
	}

	CCamera* addCamera(GameObject* gameObject)
	{
		PA_ASSERT(gameObject);
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

	// CModel* addModel(GameObject* gameObject, CModel* model)
	// {
	// 	PA_ASSERT(gameObject);
	// 	CModel* newModel = NULL;
	// 	if(!hasComponent(gameObject, Component::MODEL))
	// 	{
	// 		int index = Renderer::Model::create(model);
	// 		gameObject->compIndices[(int)Component::MODEL] = index;
	// 		Log::message("Model added to " + gameObject->name);
	// 		newModel = Renderer::Model::getModelAtIndex(index);
	// 		newModel->node = gameObject->node;
	// 	}
	// 	else
	// 	{
	// 		Log::warning("Removing existing Model from " + gameObject->name);			
	// 		removeComponent(gameObject, Component::MODEL);
	// 		newModel = addModel(gameObject, model);
	// 	}
	// 	return newModel;		
	// }

	CModel* addModel(GameObject* gameObject, const char* filename)
	{
		PA_ASSERT(gameObject);
		CModel* newModel = NULL;
		if(!hasComponent(gameObject, Component::MODEL))
		{
			int index = Renderer::Model::create(filename);
			if(index != -1)
			{
				gameObject->compIndices[Component::MODEL] = index;
				Log::message("Model added to " + gameObject->name);
				newModel = Renderer::Model::getModelAtIndex(index);
				newModel->node = gameObject->node;
			}
			else
			{
				Log::error("GO::addModel", "Model component not added to " + gameObject->name);
			}
		}
		else
		{
			Log::warning("Removing existing Model from " + gameObject->name);
			removeComponent(gameObject, Component::MODEL);
			newModel = addModel(gameObject, filename);
		}
		return newModel;
	}

	CModel* getModel(GameObject* gameObject)
	{
		PA_ASSERT(gameObject);
		CModel* model = NULL;
		if(hasComponent(gameObject, Component::MODEL))
			model = Renderer::Model::getModelAtIndex(gameObject->compIndices[(Component::MODEL)]);
		else
			Log::error("GO::getModel", gameObject->name + " does not have model component");

		return model;
	}

	CCamera* getCamera(GameObject* gameObject)
	{
		PA_ASSERT(gameObject);
		CCamera* camera = NULL;
		if(hasComponent(gameObject, Component::CAMERA))
			camera = Renderer::Camera::getCameraAtIndex(gameObject->compIndices[(Component::CAMERA)]);
		else
			Log::error("GO::getCamera", gameObject->name + " does not have camera component");

		return camera;
	}


	CTransform* getTransform(GameObject* gameObject)
	{
		PA_ASSERT(gameObject);
		CTransform* transform = NULL;		
		if(hasComponent(gameObject, Component::TRANSFORM))
			transform = Transform::getTransformAtIndex(gameObject->compIndices[(Component::TRANSFORM)]);
		else
			Log::error("GO::getTransform", gameObject->name + " does not have transform component");

		return transform;
	}

	CLight* getLight(GameObject* gameObject)
	{
		PA_ASSERT(gameObject);
		CLight* light = NULL;		
		if(hasComponent(gameObject, Component::LIGHT))
			light =Renderer::Light::getLightAtIndex(gameObject->compIndices[(Component::LIGHT)]);
		else
			Log::error("GO::getLight", gameObject->name + " does not have light component");

		return light;
	}

	void removeComponent(GameObject* gameObject, Component type)
	{
	    PA_ASSERT(gameObject);
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
