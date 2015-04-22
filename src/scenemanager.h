#ifndef _scenemanager_H
#define _scenemanager_H

#include <algorithm>
#include <vector>

#include "datatypes.h"

struct GameObject;

namespace SceneManager
{
	bool remove(const std::string& name);
	bool remove(Node node);
	bool loadScene(const std::string& filename);
	
	GameObject* find(const std::string& name);
	GameObject* find(Node node);
	GameObject* create(const std::string& name);
	GameObject* createFromFile(const std::string& name);
		
	void update();
	void cleanup();
	void generateBindings();

	std::vector<Node>* getSceneObjects();
}

#endif
