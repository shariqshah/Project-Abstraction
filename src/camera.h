#ifndef _camera_H_
#define _camera_H_

#include "renderer.h"
#include "settings.h"
#include "input.h"

class Camera
{
private:
	Node mCamParentNode;
	Node mCamNode;
	glm::vec3 mPosition;
	glm::vec3 mRotation;
	float mNearZ, mFarZ, mFov, mAspectRatio;

	void initCamera(glm::vec3 position,
					float nearZ,
					float farZ,
					float aspectRatio,
					float fov);
public:
	Camera();
	Camera(glm::vec3 position,
		   float nearZ,
		   float farZ,
		   float aspectRatio,
		   float fov);
	
	void update(float deltaTime,SDL_Event *event);
	void handleKeyboard(SDL_KeyboardEvent *key, float deltaTime);
	H3DNode getCameraNode();

	void setFOV(float fov);
	void setNearZ(float nearZ);
	void setFarZ(float farZ);
	void setAspectRatio(float aspectRatio);
	void setPosition(glm::vec3 position);

	float getFOV();
	float getFarZ();
	float getNearZ();
	float getAspectRatio();
	glm::vec3 getPosition();
};

#endif
