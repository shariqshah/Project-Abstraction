#include "camera.h"

Camera::Camera(Node parent)
{
	initCamera(0.1f,
			   1000.f,
			   4.f / 3.f,
			   75.f,
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

	if(mCamNode == 0)
		mValid = false;
	else
	{
		if(!Renderer::setParent(mCamNode, parent))
			mValid = false;
		else
		{
			Renderer::Camera::setViewportPos(mCamNode, 0, 0);
			Renderer::Camera::setViewportSize(mCamNode,
											  Settings::getWindowWidth(),
											  Settings::getWindowHeight());
			Renderer::Camera::setView(mCamNode, mFov, mAspectRatio, mNearZ, mFarZ);
		}
	}	
}

Camera::~Camera()
{
	Renderer::removeCamera(mCamNode);
	
	if(!Renderer::removeNode(mCamNode))
		Log::warning("Camera node does not exist so not removed!");
}

Node Camera::getCameraNode()
{
	return mCamNode;
}

void Camera::setFOV(float fov)
{
	mFov = fov;
	Renderer::Camera::setView(mCamNode, mFov, mAspectRatio, mNearZ, mFarZ);
}

void Camera::setNearZ(float nearZ)
{
	mNearZ = nearZ;
	Renderer::Camera::setView(mCamNode, mFov, mAspectRatio, mNearZ, mFarZ);
}

void Camera::setFarZ(float farZ)
{
	mFarZ = farZ;
	Renderer::Camera::setView(mCamNode, mFov, mAspectRatio, mNearZ, mFarZ);
}

void Camera::setAspectRatio(float aspectRatio)
{
	mAspectRatio = aspectRatio;
	Renderer::Camera::setView(mCamNode, mFov, mAspectRatio, mNearZ, mFarZ);
}

float Camera::getFOV()
{
	return mFov;
}

float Camera::getFarZ()
{
	return mFarZ;
}

float Camera::getNearZ()
{
	return mNearZ;
}

float Camera::getAspectRatio()
{
	return mAspectRatio;
}

void Camera::resizeViewport(int width, int height)
{
	Renderer::Camera::setViewportSize(mCamNode, width, height);
}

void Camera::setViewportPos(int x, int y)
{
	Renderer::Camera::setViewportPos(mCamNode, x, y);
}
