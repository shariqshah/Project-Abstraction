#include "camera.h"

Camera::Camera(Node parent)
{
	initCamera(0.1f,
			   1000.f,
			   4.f / 3.f,
			   60.f,
			   parent);
}

Camera::Camera(float nearZ,
			   float farZ,
			   float aspectRatio,
			   float fov,
			   Node  parent)
{
	initCamera(nearZ,
			   farZ,
			   aspectRatio,
			   fov,
			   parent);
}

void Camera::initCamera(float nearZ,
						float farZ,
						float aspectRatio,
						float fov,
						Node  parent)
{
	mNearZ = nearZ;
	mFarZ = farZ;
	mAspectRatio = aspectRatio;
	mFov = fov;
	mType = ComponentType::CAMERA;
	mName = "Camera";
	mCamNode = Renderer::createCamera("CameraMainNode");
	Renderer::setParent(mCamNode, parent);
	
	// Resize viewport
	Renderer::setNodeParam(mCamNode, H3DCamera::ViewportXI, 0);
	Renderer::setNodeParam(mCamNode, H3DCamera::ViewportYI, 0);
	Renderer::setNodeParam(mCamNode,
					       H3DCamera::ViewportWidthI,
					       Settings::getWindowWidth());
	Renderer::setNodeParam(mCamNode,
					       H3DCamera::ViewportHeightI,
					       Settings::getWindowHeight());
	
	// Set virtual camera parameters
	Renderer::setCameraView(mCamNode, mFov, mAspectRatio, mNearZ, mFarZ);
	Renderer::resizePipelineBuffers(Settings::getWindowWidth(),
							        Settings::getWindowHeight());
	
}

Node Camera::getCameraNode()
{
	return mCamNode;
}
