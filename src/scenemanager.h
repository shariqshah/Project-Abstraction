#ifndef _scenemanager_H
#define _scenemanager_H

#include "gameobject.h"
#include "transform.h"

typedef std::shared_ptr<GameObject> GOPtr;
typedef std::unordered_map<Node, GOPtr> SceneObjectMap;

namespace SceneManager
{
	bool add(GOPtr newGameObject);
	bool remove(std::string name);
	GOPtr find(std::string name);
	void update();
	void cleanup();
	GOPtr createGameObject(const std::string name);
	SceneObjectMap* getSceneObjects();
}

#endif
