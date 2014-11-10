#ifndef _scenemanager_H
#define _scenemanager_H

#include <algorithm>
#include <unordered_map>

#include "componentmanager.h"

typedef GameObject*                     GOPtr;
typedef std::unordered_map<Node, GOPtr> GOMap;
typedef std::vector<GOPtr>              GOArray;

namespace SceneManager
{
	bool   add(GOPtr newGameObject);
	bool   remove(const std::string& name);
	bool   remove(Node node);
	bool   setParent(GameObject* child, GameObject* parent);
	bool   setParentAsRoot(GameObject* gameObject);
	bool   getChildren(GameObject* gameObject,
					   GOArray* children,
					   const std::string& name = "");
	
	GOPtr  find(const std::string& name);
	GOPtr  find(Node node);
	GOPtr  createGameObject(const std::string& name);
	GOPtr  getParent(GameObject* gameObject);
	GOPtr  getChild(GameObject* gameObject, const std::string& name);
	
	void   update();
	void   cleanup();

	GOMap* getSceneObjects();
}

#endif
