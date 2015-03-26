#include "camera.h"
#include "scriptengine.h"
#include "transform.h"
#include "gameobject.h"
#include "scenemanager.h"
#include "passert.h"

namespace Renderer
{
	namespace
	{
		std::vector<Node>    cameras;
		std::vector<CCamera> cameraList;
		std::vector<int>     emptyIndices;
		int                  activeCameraIndex;
		
		void updateViewProjection(CCamera* camera)
		{
			camera->viewProjMat = camera->projMat * camera->viewMat;
		}
	}
	
	namespace Camera
	{
		void initialize()
		{
			activeCameraIndex = -1;
		}

		CCamera* getActiveCamera()
		{
			return getCameraAtIndex(activeCameraIndex);
		}
		
		void setActiveCamera(CCamera* camera)
		{
			if(camera)
			{
				for(int i = 0; i < (int)cameraList.size(); i++)
				{
					if(camera->node == cameraList[i].node)
					{
						activeCameraIndex = i;
						break;
					}
				}
			}
			else
			{
				activeCameraIndex = -1;
			}
		}
		
		void setFarZ(CCamera* camera, float farZ)
		{
			camera->farZ = farZ;
			updateProjection(camera);
		}

		void setNearZ(CCamera* camera, float nearZ)
		{
			camera->nearZ = nearZ;
			updateProjection(camera);
		}

		void setFov(CCamera* camera, float fov)
		{
			camera->fov = fov;
			updateProjection(camera);
		}

		void setAspectRatio(CCamera* camera, float aspectRatio)
		{
			camera->aspectRatio = aspectRatio;
			updateProjection(camera);
		}

		void updateAllCameraViews()
		{
			for(CCamera& camera : cameraList)
			{
				GameObject* gameObject = SceneManager::find(camera.node);
				CTransform* transform = GO::getTransform(gameObject);
				if(transform->isModified) updateView(&camera, transform);
			}
		}
		

		void generateBindings()
		{
			asIScriptEngine* engine = ScriptEngine::getEngine();
			int rc = -1;
			rc = engine->RegisterObjectType("Camera", sizeof(CCamera), asOBJ_REF | asOBJ_NOCOUNT); PA_ASSERT(rc >= 0);
			rc = engine->RegisterObjectProperty("Camera", "int32 node", asOFFSET(CCamera, node));
			PA_ASSERT(rc >= 0);
			rc = engine->RegisterObjectProperty("Camera", "float nearZ", asOFFSET(CCamera, nearZ));
			PA_ASSERT(rc >= 0);
			rc = engine->RegisterObjectProperty("Camera", "float farZ", asOFFSET(CCamera, farZ));
			PA_ASSERT(rc >= 0);
			rc = engine->RegisterObjectProperty("Camera", "float fov", asOFFSET(CCamera, fov));
			PA_ASSERT(rc >= 0);
			rc = engine->RegisterObjectProperty("Camera", "float aspectRatio", asOFFSET(CCamera, aspectRatio));
			PA_ASSERT(rc >= 0);
			rc = engine->RegisterObjectMethod("Camera",
											  "void setNearZ(float)",
											  asFUNCTION(setNearZ),
											  asCALL_CDECL_OBJFIRST);
			PA_ASSERT(rc >= 0);
			rc = engine->RegisterObjectMethod("Camera",
											  "void setFarZ(float)",
											  asFUNCTION(setFarZ),
											  asCALL_CDECL_OBJFIRST);
			PA_ASSERT(rc >= 0);
			rc = engine->RegisterObjectMethod("Camera",
											  "void setFov(float)",
											  asFUNCTION(setFov),
											  asCALL_CDECL_OBJFIRST);
			PA_ASSERT(rc >= 0);
			rc = engine->RegisterObjectMethod("Camera",
											  "void setAspectratio(float)",
											  asFUNCTION(setAspectRatio),
											  asCALL_CDECL_OBJFIRST);
			PA_ASSERT(rc >= 0);
			rc = engine->RegisterObjectMethod("Camera",
											  "void updateView(Transform &in)",
											  asFUNCTION(updateView),
											  asCALL_CDECL_OBJFIRST);
			PA_ASSERT(rc >= 0);
			rc = engine->RegisterObjectMethod("Camera",
											  "void updateProjection()",
											  asFUNCTION(updateView),
											  asCALL_CDECL_OBJFIRST);
			PA_ASSERT(rc >= 0);
			engine->SetDefaultNamespace("Renderer");
			rc = engine->RegisterGlobalFunction("Camera@ getActiveCamera()",
												asFUNCTION(getActiveCamera),
												asCALL_CDECL);
			PA_ASSERT(rc >= 0);
			rc = engine->RegisterGlobalFunction("void setActiveCamera(Camera@)",
												asFUNCTION(setActiveCamera),
												asCALL_CDECL);
			PA_ASSERT(rc >= 0);
			engine->SetDefaultNamespace("");
		}

		CCamera* getCameraAtIndex(int cameraIndex)
		{
			CCamera* camera = NULL;
			if(cameraIndex >= 0 && cameraIndex < (int)cameraList.size())
				camera = &cameraList[cameraIndex];
			else
				Log::error("Camera::getCameraAtIndex", "Invalid cameraIndex " + std::to_string(cameraIndex));
			return camera;
		}

		void updateAllCamerasAspectRatio(float aspectRatio)
		{
			for(CCamera& camera : cameraList)
				setAspectRatio(&camera, aspectRatio);
		}

		void updateProjection(CCamera* camera)
		{
			PA_ASSERT(camera);
			camera->projMat = glm::perspective(glm::radians(camera->fov),
											   camera->aspectRatio,
											   camera->nearZ,
											   camera->farZ);
			updateViewProjection(camera);
		}

		void updateView(CCamera* camera, CTransform* transform)
		{
			PA_ASSERT(camera);
			PA_ASSERT(transform);
			camera->viewMat = glm::lookAt(transform->position,
										  transform->lookAt,
										  transform->up);
			updateViewProjection(camera);
		}
		
		int create(GameObject* gameObject)
		{
			PA_ASSERT(gameObject);
			CCamera newCamera;
			newCamera.node = gameObject->node;
			CTransform* transform = GO::getTransform(gameObject);
			updateView(&newCamera, transform);
			updateProjection(&newCamera);			
			int index = -1;
			if(emptyIndices.empty())
			{
				cameraList.push_back(newCamera);
				index = cameraList.size() - 1;
			}
			else
			{
				index = emptyIndices.back();
				emptyIndices.pop_back();
				cameraList[index] = newCamera;
			}
			return index;
		}

		bool remove(int cameraIndex)
		{
			bool alreadyRemoved = true;
			for(unsigned int i = 0; i < emptyIndices.size(); i++)
			{
				if(emptyIndices[i] == cameraIndex)
				{
					alreadyRemoved = true;
					break;
				}
			}
			if(!alreadyRemoved)
				emptyIndices.push_back(cameraIndex);
			else
				Log::warning("Camera at index " + std::to_string(cameraIndex) + " already removed!");
			return alreadyRemoved ? false : true;
		}

		void cleanup()
		{
			cameraList.clear();
			emptyIndices.clear();
		}
	}
}

