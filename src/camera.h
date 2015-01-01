#ifndef _camera_H
#define _camera_H

#include "componentTypes.h"
#include "renderer.h"
#include "mathdefs.h"

struct CCamera
{
	Node     node        = 0;
	float    nearZ       = 0.1f;
	float    farZ        = 1000.f;
	float    fov         = 75.f;
	float    aspectRatio = 4.f/3.f;
	Pipeline pipeline    = Pipeline::FORWARD;
	bool     valid       = true;
	Mat4     viewMat;
	Mat4     projectionMat;
};

namespace Renderer
{
	namespace Camera
	{
		void initialize();
		void setViewportSize(CCamera* camera, int width, int height);
		void setViewportPos(CCamera* camera, int x, int y);
		void setOcclusionCulling(CCamera* camera, bool enable);
		void setOrthographic(CCamera* camera, bool enable);
		void updateView(CCamera* camera);
		void removeCamera(const CCamera& camera);
		void resizePipelineBuffers(int width, int height);
		void setPipeline(CCamera* camera, Pipeline pipeline);
		void setAspectRatio(CCamera* camera, float aspectRatio);
		void setFov(CCamera* camera, float fov);
		void setNearZ(CCamera* camera, float nearZ);
		void setFarZ(CCamera* camera, float farZ);
		void generateBindings();
		void setOutputTexture(CCamera* camera, Resource texture);
		
		CCamera create(const std::string& name, Node parent, Resource pipeline);
	}
}

#endif
