#ifndef _motionstate_H
#define _motionstate_H

#include "../include/bullet/LinearMath/btMotionState.h"
#include "datatypes.h"

struct GameObject;

class MotionState : public btMotionState
{
	Node node;
public:
	MotionState(GameObject* gameObject);

	virtual ~MotionState();
	virtual void getWorldTransform(btTransform& worldTrans) const;
	virtual void setWorldTransform(const btTransform& worldTrans);
};

#endif
