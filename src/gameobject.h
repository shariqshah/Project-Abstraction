#ifndef _gameobject_H_
#define _gameobject_H_

#include <vector>

#include "renderer.h"
#include "componentTypes.h"

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
	void removeScript(GameObject* gameObject, const std::string& name);
	void reloadScriptByName(const std::string& goName, const std::string& name);
	void reloadAllScripts(GameObject* gameObject);
}

#endif
