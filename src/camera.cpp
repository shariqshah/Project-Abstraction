#include "camera.h"
#include "scriptengine.h"
#include "transform.h"
#include "componentmanager.h"
#include "gameobject.h"

namespace Renderer
{
	namespace
	{
		std::vector<Node>    cameras;
		std::vector<CCamera> cameraList;
		std::vector<int>     emptyIndices;
        Resource             pipelineList[4];

		void updateViewProjection(CCamera* camera)
		{
			camera->viewProjMat = camera->projMat * camera->viewMat;
		}
	}
	
	namespace Camera
	{
		void initialize()
		{
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

		void generateBindings()
		{
			Sqrat::RootTable().Bind("CCamera", Sqrat::Class<CCamera>()
								.Var("node",        &CCamera::node)
								.Var("nearZ",       &CCamera::nearZ)
								.Var("farZ",        &CCamera::farZ)
								.Var("fov",         &CCamera::fov)
								.Var("aspectRatio", &CCamera::aspectRatio)
								.Var("pipeline",    &CCamera::pipeline));

			Sqrat::RootTable().Bind("Camera", Sqrat::Table(ScriptEngine::getVM())
								.Func("setAspectratio", &setAspectRatio)
								.Func("setFov", &setFov)
								.Func("setNearZ", &setNearZ)
								.Func("updateView", &updateView)
								// .Func("removeCamera", &removeCamera)
								.Func("setFarZ", &setFarZ));
		}

		CCamera* getCameraAtIndex(int cameraIndex)
		{
			return &cameraList[cameraIndex];
		}

		void updateProjection(CCamera* camera)
		{
			assert(camera);
			
			camera->projMat = glm::perspective(glm::radians(camera->fov),
											   camera->aspectRatio,
											   camera->nearZ,
											   camera->farZ);
			updateViewProjection(camera);
		}

		void updateView(CCamera* camera, CTransform* transform)
		{
			assert(camera);
			assert(transform);
			
			camera->viewMat = glm::lookAt(transform->position,
										  transform->lookAt,
										  transform->up);

			updateViewProjection(camera);
		}
		
		int create(GameObject* gameObject)
		{
			assert(gameObject);
			
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

