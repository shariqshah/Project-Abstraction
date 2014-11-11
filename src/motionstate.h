#ifndef _motionstate_H
#define _motionstate_H

#include "../include/bullet/LinearMath/btMotionState.h"

struct GameObject;

typedef GameObject* GOPtr;

class MotionState : public btMotionState
{
	GOPtr mGameObject;
public:
	MotionState(GOPtr gameObject);

	virtual ~MotionState();

	virtual void getWorldTransform(btTransform& worldTrans) const;
	virtual void setWorldTransform(const btTransform& worldTrans);
   
};

#endif
