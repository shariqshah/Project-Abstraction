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
		camera->projMat = glm::perspective(glm::radians(camera->fov),
										   camera->aspectRatio,
										   camera->nearZ,
										   camera->farZ);
		updateViewProjection(camera);
	}

	float InvSqrt(float x)
	{
		float xhalf;
		int i;
 
		xhalf = 0.5f * x;
		i = *(int*)&x;
		i = 0x5f3759df - (i >> 1);
		x = *(float*)&i;
		x = x * (1.5f - xhalf * x * x);
		return x;
	}
	
	void updateFrustum(CCamera* camera)
	{
		PA_ASSERT(camera);
		
		float t;
		float m00,m01,m02,m03;
		float m10,m11,m12,m13;
		float m20,m21,m22,m23;
		float m30,m31,m32,m33;
		Frustum* frustum = &camera->frustum;
		GameObject* gameObject = SceneManager::find(camera->node);
		CTransform* transform  = GO::getTransform(gameObject);
		Mat4 mvp = camera->viewProjMat;// * transform->transMat;
		mvp = glm::transpose(mvp);
 
		m00 = mvp[0][0];
		m01 = mvp[0][1];
		m02 = mvp[0][2];
		m03 = mvp[0][3];
 
		m10 = mvp[1][0];
		m11 = mvp[1][1];
		m12 = mvp[1][2];
		m13 = mvp[1][3];
 
		m20 = mvp[2][0];
		m21 = mvp[2][1];
		m22 = mvp[2][2];
		m23 = mvp[2][3];
 
		m30 = mvp[3][0];
		m31 = mvp[3][1];
		m32 = mvp[3][2];
		m33 = mvp[3][3];
 
		frustum->planes[Frustum::RIGHT].x = m30 - m00;
		frustum->planes[0].y = m31 - m01;
		frustum->planes[0].z = m32 - m02;
		frustum->planes[0].w = m33 - m03;
 
		t = InvSqrt(frustum->planes[0].x * frustum->planes[0].x + frustum->planes[0].y * frustum->planes[0].y + frustum->planes[0].z * frustum->planes[0].z);
 
		frustum->planes[0].x *= t;
		frustum->planes[0].y *= t;
		frustum->planes[0].z *= t;
		frustum->planes[0].w *= t;
 
		frustum->planes[1].x = m30 + m00;
		frustum->planes[1].y = m31 + m01;
		frustum->planes[1].z = m32 + m02;
		frustum->planes[1].w = m33 + m03;
 
		t = InvSqrt(frustum->planes[1].x * frustum->planes[1].x + frustum->planes[1].y * frustum->planes[1].y + frustum->planes[1].z * frustum->planes[1].z);
 
		frustum->planes[1].x *= t;
		frustum->planes[1].y *= t;
		frustum->planes[1].z *= t;
		frustum->planes[1].w *= t;
 
		frustum->planes[2].x = m30 - m10;
		frustum->planes[2].y = m31 - m11;
		frustum->planes[2].z = m32 - m12;
		frustum->planes[2].w = m33 - m13;
 
		t = InvSqrt(frustum->planes[2].x * frustum->planes[2].x + frustum->planes[2].y * frustum->planes[2].y + frustum->planes[2].z * frustum->planes[2].z);
 
		frustum->planes[2].x *= t;
		frustum->planes[2].y *= t;
		frustum->planes[2].z *= t;
		frustum->planes[2].w *= t;
 
		frustum->planes[3].x = m30 + m10;
		frustum->planes[3].y = m31 + m11;
		frustum->planes[3].z = m32 + m12;
		frustum->planes[3].w = m33 + m13;
 
		t = InvSqrt(frustum->planes[3].x * frustum->planes[3].x + frustum->planes[3].y * frustum->planes[3].y + frustum->planes[3].z * frustum->planes[3].z);
 
		frustum->planes[3].x *= t;
		frustum->planes[3].y *= t;
		frustum->planes[3].z *= t;
		frustum->planes[3].w *= t;
 
		frustum->planes[4].x = m30 - m20;
		frustum->planes[4].y = m31 - m21;
		frustum->planes[4].z = m32 - m22;
		frustum->planes[4].w = m33 - m23;
 
		t = InvSqrt(frustum->planes[4].x * frustum->planes[4].x + frustum->planes[4].y * frustum->planes[4].y + frustum->planes[4].z * frustum->planes[4].z);
 
		frustum->planes[4].x *= t;
		frustum->planes[4].y *= t;
		frustum->planes[4].z *= t;
		frustum->planes[4].w *= t;
 
		frustum->planes[5].x = m30 + m20;
		frustum->planes[5].y = m31 + m21;
		frustum->planes[5].z = m32 + m22;
		frustum->planes[5].w = m33 + m23;
 
		t = InvSqrt(frustum->planes[5].x * frustum->planes[5].x + frustum->planes[5].y * frustum->planes[5].y + frustum->planes[5].z * frustum->planes[5].z);
 
		frustum->planes[5].x *= t;
		frustum->planes[5].y *= t;
		frustum->planes[5].z *= t;
		frustum->planes[5].w *= t;
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
