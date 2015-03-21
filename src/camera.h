#ifndef _camera_H
#define _camera_H

#include "componentTypes.h"
#include "renderer.h"
#include "mathdefs.h"
#include "datatypes.h"

struct GameObject;
struct CTransform;

struct CCamera
{
	Node  node        = 0;
	float nearZ       = 0.1f;
	float farZ        = 1000.f;
	float fov         = 75.f;
	float aspectRatio = 4.f/3.f;
	bool  valid       = true;
	Mat4  viewMat;
	Mat4  projMat;
	Mat4  viewProjMat;
};

namespace Renderer
{
	namespace Camera
	{
		void     initialize();
		void     setAspectRatio(CCamera* camera, float aspectRatio);
		void     setFov(CCamera* camera, float fov);
		void     setNearZ(CCamera* camera, float nearZ);
		void     setFarZ(CCamera* camera, float farZ);
		void     generateBindings();
		void     updateProjection(CCamera* camera);
		void     updateView(CCamera* camera, CTransform* transform);
		bool     remove(int cameraIndex);
		void     cleanup();
		void     updateAllCamerasAspectRatio(float aspectRatio);		
		CCamera* getCameraAtIndex(int cameraIndex);
		int      create(GameObject* gameObject);
		void     updateAllCameraViews();
		CCamera* getActiveCamera();
		void     setActiveCamera(CCamera* camera); // Pass camera as NULL to reset active camera to none
	}
}

#endif
