#ifndef _componentmanager_H_
#define _componentmanager_H_

#include <iostream>
#include "componentTypes.h"

typedef int32_t CRigidBody;

struct CCamera;
struct CTransform;
struct CModel;
struct CLight;
struct GameObject;
class  CollisionShape;

namespace CompManager
{	
	void initailize();
	void cleanup();

	CTransform* addTransform(GameObject* gameObject);
	CCamera*    addCamera(GameObject* gameObject, const std::string& name);
	CModel*     addModel(GameObject* gameObject, const std::string& filename);
	CLight*     addLight(GameObject* gameObject, const std::string& name);
	CRigidBody  addRigidBody(GameObject* gameObject,
							 CollisionShape* shape,
							 const float mass        = 1.f,
							 const float restitution = 0.5f);
	
	CTransform* getTransform(GameObject* gameObject);
	CCamera*    getCamera(GameObject* gameObject);
	CModel*     getModel(GameObject* gameObject);
	CLight*     getLight(GameObject* gameObject);
	CRigidBody  getRigidBody(GameObject* gameObject);

	void removeComponent(GameObject* gameObject, ComponentType type);
}

#endif
