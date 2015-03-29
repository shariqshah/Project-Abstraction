#include "motionstate.h"
#include "transform.h"
#include "gameobject.h"
#include "utilities.h"
#include "log.h"
#include "scenemanager.h"

MotionState::MotionState(GameObject* gameObject)
{
	node = gameObject->node;
}

MotionState::~MotionState() {}

void MotionState::getWorldTransform(btTransform& worldTrans) const
{
	GameObject* gameobject = SceneManager::find(node);
	CTransform* transform  = GO::getTransform(gameobject);
	worldTrans.setFromOpenGLMatrix(glm::value_ptr(transform->transMat));
}

void MotionState::setWorldTransform(const btTransform& worldTrans)
{
	// update transform and reset renderer's transformation
	// flag, indicating that transform has been updated by bullet
	GameObject* gameobject = SceneManager::find(node);
	CTransform* transform  = GO::getTransform(gameobject);
	Transform::setPosition(transform, Utils::toGlm(worldTrans.getOrigin()));
	Transform::setRotation(transform, Utils::toGlm(worldTrans.getRotation()));
	transform->isModified = true;
}
