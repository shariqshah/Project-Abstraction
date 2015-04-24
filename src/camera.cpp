#include "camera.h"
#include "scriptengine.h"
#include "transform.h"
#include "gameobject.h"
#include "scenemanager.h"
#include "passert.h"

namespace
{
	std::vector<Node>    cameras;
	std::vector<CCamera> cameraList;
	std::vector<int>     emptyIndices;
	int                  activeCameraIndex;
		
	void updateViewProjection(CCamera* camera)
	{
		camera->viewProjMat = camera->projMat * camera->viewMat;
		Camera::updateFrustum(camera);
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

	void generateBindings()
	{
		asIScriptEngine* engine = ScriptEngine::getEngine();
		int rc = -1;
		rc = engine->RegisterObjectType("Camera", sizeof(CCamera), asOBJ_REF | asOBJ_NOCOUNT);
		PA_ASSERT(rc >= 0);
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
										  "void updateView()",
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
		if(aspectRatio <= 0.f) aspectRatio = 4.f / 3.f;
		for(CCamera& camera : cameraList)
			setAspectRatio(&camera, aspectRatio);
	}

	void updateProjection(CCamera* camera)
	{
		PA_ASSERT(camera);
		camera->projMat = glm::perspective(camera->fov,
										   camera->aspectRatio,
										   camera->nearZ,
										   camera->farZ);
		updateViewProjection(camera);
	}
	
	void updateFrustum(CCamera* camera)
	{
		PA_ASSERT(camera);
		Frustum* frustum = &camera->frustum;
		Mat4 mvp = camera->viewProjMat;

		frustum->planes[Frustum::LEFT].x   = mvp[0][3] + mvp[0][0];
		frustum->planes[Frustum::LEFT].y   = mvp[1][3] + mvp[1][0];
		frustum->planes[Frustum::LEFT].z   = mvp[2][3] + mvp[2][0];
		frustum->planes[Frustum::LEFT].w   = mvp[3][3] + mvp[3][0];
 
		frustum->planes[Frustum::RIGHT].x  = mvp[0][3] - mvp[0][0];
		frustum->planes[Frustum::RIGHT].y  = mvp[1][3] - mvp[1][0];
		frustum->planes[Frustum::RIGHT].z  = mvp[2][3] - mvp[2][0];
		frustum->planes[Frustum::RIGHT].w  = mvp[3][3] - mvp[3][0];
       
		frustum->planes[Frustum::BOTTOM].x = mvp[0][3] + mvp[0][1];
		frustum->planes[Frustum::BOTTOM].y = mvp[1][3] + mvp[1][1];
		frustum->planes[Frustum::BOTTOM].z = mvp[2][3] + mvp[2][1];
		frustum->planes[Frustum::BOTTOM].w = mvp[3][3] + mvp[3][1];
 
		frustum->planes[Frustum::TOP].x    = mvp[0][3] - mvp[0][1];
		frustum->planes[Frustum::TOP].y    = mvp[1][3] - mvp[1][1];
		frustum->planes[Frustum::TOP].z    = mvp[2][3] - mvp[2][1];
		frustum->planes[Frustum::TOP].w    = mvp[3][3] - mvp[3][1];
 
		frustum->planes[Frustum::NEAR].x   = mvp[0][3] + mvp[0][2];
		frustum->planes[Frustum::NEAR].y   = mvp[1][3] + mvp[1][2];
		frustum->planes[Frustum::NEAR].z   = mvp[2][3] + mvp[2][2];
		frustum->planes[Frustum::NEAR].w   = mvp[3][3] + mvp[3][2];
       
		frustum->planes[Frustum::FAR].x    = mvp[0][3] - mvp[0][2];
		frustum->planes[Frustum::FAR].y    = mvp[1][3] - mvp[1][2];
		frustum->planes[Frustum::FAR].z    = mvp[2][3] - mvp[2][2];
		frustum->planes[Frustum::FAR].w    = mvp[3][3] - mvp[3][2];

		for(int i = 0; i < 6; i++)
		{
			float length = glm::length(glm::vec3(frustum->planes[i]));
			frustum->planes[i] /= length;
		}
	}
	
	void updateView(CCamera* camera)
	{
		PA_ASSERT(camera);
		GameObject* gameObject = SceneManager::find(camera->node);
		CTransform* transform  = GO::getTransform(gameObject);
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
		updateView(&newCamera);
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
		bool alreadyRemoved = false;
		for(unsigned int i = 0; i < emptyIndices.size(); i++)
		{
			if(emptyIndices[i] == cameraIndex)
			{
				alreadyRemoved = true;
				break;
			}
		}
		if(!alreadyRemoved)
		{
			emptyIndices.push_back(cameraIndex);
			if(cameraIndex == activeCameraIndex) activeCameraIndex = -1;
		}
		else
			Log::warning("Camera at index " + std::to_string(cameraIndex) + " already removed!");
		return alreadyRemoved ? false : true;
	}

	void cleanup()
	{
		cameraList.clear();
		emptyIndices.clear();
	}

	bool writeToJSON(CCamera* camera, rapidjson::Writer<rapidjson::StringBuffer>& writer)
	{
		using namespace rapidjson;
		bool success = true;
		writer.Key("Camera");
		writer.StartObject();
		writer.Key("NearZ");       writer.Double(camera->nearZ);
		writer.Key("FarZ");        writer.Double(camera->farZ);
		writer.Key("Fov");         writer.Double(camera->fov);
		writer.Key("AspectRatio"); writer.Double(camera->aspectRatio);
		writer.Key("IsActive");    writer.Bool(camera->node == getActiveCamera()->node ? true : false);
		writer.EndObject();
		return success;
	}

	bool createFromJSON(CCamera* camera, const rapidjson::Value& value)
    {
		using namespace rapidjson;
		bool success = true;
		const char* error = "Invalid value in a field";
		PA_ASSERT(camera);

		if(value.IsObject())
		{
			if(value.HasMember("NearZ") && value["NearZ"].IsNumber())
			{
				const Value& nearZNode = value["NearZ"];
				float nearZ = (float)nearZNode.GetDouble();
				if(nearZ > 0)
					camera->nearZ = nearZ;
				else
					success = false;
			}
			else
			{
				success = false;
			}

			if(value.HasMember("FarZ") && value["FarZ"].IsNumber())
			{
				const Value& farZNode = value["FarZ"];
				float farZ = (float)farZNode.GetDouble();
				if(farZ > 0)
					camera->farZ = farZ;
				else
					success = false;
			}
			else
			{
				success = false;
			}

			if(value.HasMember("Fov") && value["Fov"].IsNumber())
			{
				const Value& fovNode = value["Fov"];
				float fov = (float)fovNode.GetDouble();
				if(fov > 0)
					camera->fov = glm::radians(fov);
				else
					success = false;
			}
			else
			{
				success = false;
			}

			if(value.HasMember("AspectRatio") && value["AspectRatio"].IsNumber())
			{
				const Value& aspectRatioNode = value["AspectRatio"];
				float aspectRatio = (float)aspectRatioNode.GetDouble();
				if(aspectRatio > 0)
					camera->aspectRatio = aspectRatio;
				else
					success = false;
			}
			else
			{
				success = false;
			}

			if(value.HasMember("IsActive") && value["IsActive"].IsBool())
			{
				const Value& IsActiveNode = value["IsActive"];
				if(IsActiveNode.GetBool())
					Camera::setActiveCamera(camera);
			}
			else
			{
				success = false;
			}
			updateView(camera);
			updateProjection(camera);
		}
		else
		{
			error   = "'Camera' must be an object";
			success = false;
		}
		if(!success) Log::error("Camera::createFromJSON", error);
		return success;
	}
}
