#ifndef _camera_H_
#define _camera_H_

#include "component.h"
#include "renderer.h"

class Camera : public Component
{
private:
	Node  mCamNode;
	float mNearZ, mFarZ, mFov, mAspectRatio;
	Pipeline mPipeline;
	
	void initCamera(float nearZ,
					float farZ,
					float aspectRatio,
					float fov,
					Node Parent);
public:
	Camera(Node parent);
	Camera(float nearZ,
		   float farZ,
		   float aspectRatio,
		   float fov,
		   Node Parent);
	~Camera();
	
	Node getCameraNode();

	void setFOV(float fov);
	void setNearZ(float nearZ);
	void setFarZ(float farZ);
	void setAspectRatio(float aspectRatio);
	void resizeViewport(int width, int height);
	void setViewportPos(int x, int y);
	void setPipeline(Pipeline pipeline);

	float    getFOV();
	float    getFarZ();
	float    getNearZ();
	float    getAspectRatio();
	Pipeline getPipeline();
};

#endif
