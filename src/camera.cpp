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
            pipelineList[0] = Resources::add(ResourceType::PIPELINE,
										   "pipelines/forward.pipeline.xml",
										   0);
            pipelineList[1] = Resources::add(ResourceType::PIPELINE,
										   "pipelines/deferred.pipeline.xml",
										   0);
            pipelineList[2] = Resources::add(ResourceType::PIPELINE,
										   "pipelines/hdr.pipeline.xml",
										   0);
            pipelineList[3] = Resources::add(ResourceType::PIPELINE,
										   "pipelines/rtt.pipeline.xml",
										   0);

			Resources::loadAddedResources();
		}

		CCamera create(const std::string& name, Node parent, Resource pipeline)
		{
			CCamera newCamera;
			Node node = h3dAddCameraNode(parent,
										 name.c_str(),
										 pipelineList[(int)pipeline]);
			if(node != 0)
			{
				cameras.push_back(node);
				newCamera.node = node;

				setViewportPos(&newCamera, 0, 0);
				setViewportSize(&newCamera,
								Settings::getWindowWidth(),
								Settings::getWindowHeight());
				updateView(&newCamera);
				setOcclusionCulling(&newCamera, true);

				return newCamera;
			}
			
			newCamera.valid = false;
			return newCamera;
		}

		void removeCamera(const CCamera& camera)
		{
			if(!cameras.empty())
			{
				cameras.erase(std::find(cameras.begin(),
										cameras.end(),
										camera.node),
							  cameras.end());

				Renderer::removeNode(camera.node);
			}
			else
				Log::error("Camera", "Could not remove camera. No cameras in scene!");
		}

		void resizePipelineBuffers(int width, int height)
		{
            h3dResizePipelineBuffers(pipelineList[(int)Pipeline::FORWARD],
									 width,
									 height);
			
            h3dResizePipelineBuffers(pipelineList[(int)Pipeline::DEFERRED],
									 width,
									 height);
			
            h3dResizePipelineBuffers(pipelineList[(int)Pipeline::HDR],
									 width,
									 height);
		}
		
		void setViewportSize(CCamera* camera, int width, int height)
		{
			h3dSetNodeParamI(camera->node, H3DCamera::ViewportWidthI, width);
			h3dSetNodeParamI(camera->node, H3DCamera::ViewportHeightI, height);
	    }

		void setViewportPos(CCamera* camera, int x, int y)
		{
			h3dSetNodeParamI(camera->node, H3DCamera::ViewportXI, x);
			h3dSetNodeParamI(camera->node, H3DCamera::ViewportYI, y);
		}

		void updateView(CCamera* camera)
		{
			h3dSetupCameraView(camera->node,
							   camera->fov,
							   camera->aspectRatio,
							   camera->nearZ,
							   camera->farZ);
		}

		void setFarZ(CCamera* camera, float farZ)
		{
			camera->farZ = farZ;
			updateView(camera);
		}

		void setNearZ(CCamera* camera, float nearZ)
		{
			camera->nearZ = nearZ;
			updateView(camera);
		}

		void setFov(CCamera* camera, float fov)
		{
			camera->fov = fov;
			updateView(camera);
		}

		void setAspectRatio(CCamera* camera, float aspectRatio)
		{
			camera->aspectRatio = aspectRatio;
			updateView(camera);
		}

		void setPipeline(CCamera* camera, Pipeline pipeline)
		{
			Resource newPipeline = 0;
			camera->pipeline = pipeline;
			
			switch(pipeline)
			{
			case Pipeline::FORWARD:
                newPipeline = pipelineList[(int)Pipeline::FORWARD];
				break;
			case Pipeline::DEFERRED:
                newPipeline = pipelineList[(int)Pipeline::DEFERRED];
				break;
			case Pipeline::HDR:
                newPipeline = pipelineList[(int)Pipeline::HDR];
				break;
			case Pipeline::RTT:
                newPipeline = pipelineList[(int)Pipeline::RTT];
				break;
			}

			h3dSetNodeParamI(camera->node, H3DCamera::PipeResI, newPipeline);
		}

		void setOcclusionCulling(CCamera* camera, bool enable)
		{
			h3dSetNodeParamI(camera->node, H3DCamera::OccCullingI, enable ? 1 : 0);
		}

		void setOrthographic(CCamera* camera, bool enable)
		{
			h3dSetNodeParamI(camera->node, H3DCamera::OrthoI, enable ? 1 : 0);
		}

		void setOutputTexture(CCamera* camera, Resource texture)
		{
			h3dSetNodeParamI(camera->node, H3DCamera::OutTexResI, texture);
		}

		void generateBindings()
		{
			Sqrat::ConstTable().Enum("Pipeline", Sqrat::Enumeration ()
									 .Const("FORWARD",  Pipeline::FORWARD)
									 .Const("DEFERRED", Pipeline::DEFERRED)
									 .Const("HDR",      Pipeline::HDR));

			Sqrat::RootTable().Bind("CCamera", Sqrat::Class<CCamera>()
								.Var("node",        &CCamera::node)
								.Var("nearZ",       &CCamera::nearZ)
								.Var("farZ",        &CCamera::farZ)
								.Var("fov",         &CCamera::fov)
								.Var("aspectRatio", &CCamera::aspectRatio)
								.Var("pipeline",    &CCamera::pipeline));

			Sqrat::RootTable().Bind("Camera", Sqrat::Table(ScriptEngine::getVM())
								.Func("setViewportSize", &setViewportSize)
								.Func("setViewportPos", &setViewportPos)
								.Func("setPipeline", &setPipeline)
								.Func("setCulling", &setOcclusionCulling)
								.Func("setOrthgraphic", &setOrthographic)
								.Func("setAspectratio", &setAspectRatio)
								.Func("setFov", &setFov)
								.Func("setNearZ", &setNearZ)
								// .Func("updateView", &updateView)
								.Func("removeCamera", &removeCamera)
								.Func("setFarZ", &setFarZ));
		}

		CCamera* getCamera(int cameraIndex)
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
		
		CCamera* create(GameObject* gameObject, CCamera* camera)
		{
			assert(gameObject);
			assert(camera);

			camera->node = gameObject->node;
			CTransform* transform = CompManager::getTransform(gameObject);
			
			updateView(camera, transform);
			updateProjection(camera);

			int index = -1;

			if(emptyIndices.empty())
			{
				cameraList.push_back(*camera);
				index = cameraList.size() - 1;
			}
			else
			{
				index = emptyIndices.back();
				emptyIndices.pop_back();
				cameraList[index] = *camera;
			}

			gameObject->compIndices[(int)Component::CAMERA] = index;
			Log::message("Camera added to " + gameObject->name);
			return &cameraList[index];
		}

		void remove(int cameraIndex)
		{
			emptyIndices.push_back(cameraIndex);
		}
	}
}

