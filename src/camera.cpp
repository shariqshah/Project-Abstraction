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

// void Camera::handleKeyboard(SDL_KeyboardEvent *key, float deltaTime)
// {
// 	float increment = 20.f * deltaTime;
// 	glm::vec3 translation(0.f);

// 	if(Input::isPressed(Input::Key::W))
// 		translation.z -= increment;
// 	if(Input::isPressed(Input::Key::S))
// 		translation.z += increment;
// 	if(Input::isPressed(Input::Key::A))
// 		translation.x -= increment;
// 	if(Input::isPressed(Input::Key::D))
// 		translation.x += increment;
// 	if(Input::isPressed(Input::Key::Q))
// 		translation.y += increment;
// 	if(Input::isPressed(Input::Key::E))
// 		translation.y -= increment;
	
// 	if(Input::isPressed(Input::MouseButton::LEFT))
// 		translation.z -= increment;
// 	if(Input::isReleased(Input::MouseButton::RIGHT))
// 		translation.y += 10;

// 	glm::quat rotationQuat(glm::vec3(glm::radians(mRotation.x),
// 									 glm::radians(mRotation.y),
// 									 0));
// 	mPosition += (rotationQuat * translation);
	
// }


// void Camera::update(float deltaTime, SDL_Event *event)
// {
// 	handleKeyboard(&event->key, deltaTime);
	
// 	h3dSetNodeTransform(mCamNode,
// 						0, 0, 0,
// 						mRotation.x, 0, 0,
// 						1, 1, 1);

// 	h3dSetNodeTransform(mCamParentNode,						   
// 						mPosition.x, mPosition.y, mPosition.z,
// 						0, mRotation.y, 0,					   
// 						1, 1, 1);							   
// }
