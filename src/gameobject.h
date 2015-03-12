#ifndef _gameobject_H_
#define _gameobject_H_

#include <vector>
#include <iostream>

#include "componentTypes.h"

typedef uint32_t Node;

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
	
	int compIndices[6] = {-1, -1, -1, -1, -1, -1};
};

namespace GO
{
    bool hasComponent(GameObject* gameOjbject, Component type);
	void generateBindings();
	void attachScript(GameObject* gameObject, const std::string& name);
	void reloadScript(GameObject* gameObject, const std::string& name);
	void reloadScript(const std::string& goName, const std::string& name);
	void reloadAllScripts(GameObject* gameObject);
	void reloadAllScripts(const std::string& goName);
	void reloadScriptType(const std::string& typeName);
	void removeScript(GameObject* gameObject, const std::string& name);
	void removeScript(const std::string& goName, const std::string& name);
	void processCollision(GameObject* gameObject, const CollisionData& collisionData);
	void removeComponent(GameObject* gameObject, Component type);

	CTransform* addTransform(GameObject* gameObject);
	CCamera*    addCamera(GameObject* gameObject);
	// CModel*     addModel(GameObject* gameObject, CModel* model);
	CModel*     addModel(GameObject* gameObject, const char* filename);
	CLight*     addLight(GameObject* gameObject);
	
	CTransform* getTransform(GameObject* gameObject);
	CCamera*    getCamera(GameObject* gameObject);
	CModel*     getModel(GameObject* gameObject);
	CLight*     getLight(GameObject* gameObject);
}

#endif
