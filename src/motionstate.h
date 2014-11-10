#ifndef _motionstate_H
#define _motionstate_H

#include "componentmanager.h"

typedef GameObject* GOPtr;

class MotionState : public btMotionState
{
	GOPtr mGameObject;

public:
	MotionState(GOPtr gameObject)
	{
		mGameObject = gameObject;
	}

	virtual ~MotionState() {}

	virtual void getWorldTransform(btTransform& worldTrans) const
	{
		if(mGameObject)
		{
			auto transform = CompManager::getTransform(mGameObject);
			if(transform)
				worldTrans.setFromOpenGLMatrix(glm::value_ptr(transform->transMat));
			else
				Log::warning("Transform in motion state object is NULL");
		}
		else
			Log::warning("GameObject in motion state is NULL");
	}

	virtual void setWorldTransform(const btTransform& worldTrans)
	{
		
		if(mGameObject)
		{
			auto transform = CompManager::getTransform(mGameObject);
			if(transform)
			{
				// update transform and reset renderer's transformation
				// flag, indicating that transform has been updated by bullet
				Transform::setPosition(transform,
									   Utils::toGlm(worldTrans.getOrigin()));
				Transform::setRotation(transform,
									   Utils::toGlm(worldTrans.getRotation()));
				Transform::resetTransformFlag(transform);
			}
			else
				Log::warning("Transform in motion state object is NULL");
		}
		else
			Log::warning("GameObject in motion state is NULL");
	}
   
};

#endif
