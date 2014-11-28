#ifndef _gameobject_H_
#define _gameobject_H_

#include <vector>

#include "renderer.h"
#include "componentTypes.h"

static const int EMPTY_INDEX = -1;

struct GameObject
{
	Node        node   = 0;
	std::string name   = "DefaultGameobjectName";
	std::string tag    = "DefaultTag";
	bool        remove = false;
	
	int compIndices[6] = {-1, -1, -1, -1, -1, -1};
	std::vector<int> scripts;
};

namespace GO
{
    bool hasComponent(GameObject* gameOjbject, Component type);
	void generateBindings();
	void attachScript(GameObject* gameObject, const std::string& name);
	void updateScripts(GameObject* gameObject, float deltaTime);
}

#endif
