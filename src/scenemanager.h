#ifndef _scenemanager_H
#define _scenemanager_H

#include <algorithm>
#include <vector>

struct GameObject;

typedef uint32_t    Node;
typedef GameObject* GOPtr;

namespace SceneManager
{
	bool remove(const std::string& name);
	bool remove(Node node);
	
	GOPtr find(const std::string& name);
	GOPtr find(Node node);
	GOPtr create(const std::string& name);

	void update();
	void cleanup();
	void generateBindings();

	std::vector<Node>* getSceneObjects();
}

#endif
