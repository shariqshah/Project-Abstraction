#ifndef _componentmanager_H_
#define _componentmanager_H_

#include "gameobject.h"
#include "camera.h"
#include "model.h"
#include "light.h"
#include "physics.h"

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
							 float mass        = 1.f,
							 float restitution = 0.5f);
	
	CTransform* getTransform(GameObject* gameObject);
	CCamera*    getCamera(GameObject* gameObject);
	CModel*     getModel(GameObject* gameObject);
	CLight*     getLight(GameObject* gameObject);
	CRigidBody  getRigidBody(GameObject* gameObject);

	void removeComponent(GameObject* gameObject, ComponentType type);
}

#endif
