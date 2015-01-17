#include "motionstate.h"
#include "transform.h"
#include "gameobject.h"
#include "utilities.h"
#include "log.h"

MotionState::MotionState(GOPtr gameObject)
{
	mGameObject = gameObject;
}

MotionState::~MotionState() {}

void MotionState::getWorldTransform(btTransform& worldTrans) const
{
	// if(mGameObject)
	// {
	// 	auto transform = CompManager::getTransform(mGameObject);
	// 	if(transform)
	// 		worldTrans.setFromOpenGLMatrix(glm::value_ptr(transform->transMat));
	// 	else
	// 		Log::warning("Transform in motion state object is NULL");
	// }
	// else
	// 	Log::warning("GameObject in motion state is NULL");
}

void MotionState::setWorldTransform(const btTransform& worldTrans)
{		
	// if(mGameObject)
	// {
	// 	auto transform = CompManager::getTransform(mGameObject);
	// 	if(transform)
	// 	{
	// 		// update transform and reset renderer's transformation
	// 		// flag, indicating that transform has been updated by bullet
	// 		Transform::setPosition(transform,
	// 							   Utils::toGlm(worldTrans.getOrigin()));
	// 		Transform::setRotation(transform,
	// 							   Utils::toGlm(worldTrans.getRotation()));
	// 		Transform::resetTransformFlag(transform);
	// 	}
	// 	else
	// 		Log::warning("Transform in motion state object is NULL");
	// }
	// else
	// 	Log::warning("GameObject in motion state is NULL");
}


