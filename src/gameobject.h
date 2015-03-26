#ifndef _gameobject_H_
#define _gameobject_H_

#include <vector>
#include <iostream>

#include "componentTypes.h"
#include "mathdefs.h"
#include "datatypes.h"

struct CollisionData;
struct CTransform;
struct CModel;
struct CCamera;
struct CLight;

const static int EMPTY_INDEX = -1;

struct GameObject
{
	Node        node   = 0;
	std::string name   = "DefaultGameobjectName";
	std::string tag    = "DefaultTag";
	bool        remove = false;
	int         scriptIndex = -1;
	int compIndices[6] = {-1, -1, -1, -1, -1, -1};
};

namespace GO
{
    bool hasComponent(GameObject* gameOjbject, Component type);
	void generateBindings();
	void processCollision(GameObject* gameObject, const CollisionData& collisionData);
	void removeComponent(GameObject* gameObject, Component type);

	CTransform* addTransform(GameObject* gameObject);
	CCamera*    addCamera(GameObject* gameObject);
	CModel*     addModel(GameObject* gameObject, const char* filename);
	CLight*     addLight(GameObject* gameObject, Vec4 color = Vec4(1.f));
	
	CTransform* getTransform(GameObject* gameObject);
	CCamera*    getCamera(GameObject* gameObject);
	CModel*     getModel(GameObject* gameObject);
	CLight*     getLight(GameObject* gameObject);
}

#endif
