#ifndef _gameobject_H_
#define _gameobject_H_

#include <array>

#include "renderer.h"
#include "componentTypes.h"

static const int EMPTY_INDEX = -1;

struct GameObject
{
	Node        node   = 0;
	std::string name   = "DefaultGameobjectName";
	std::string tag    = "DefaultTag";
	bool        remove = false;
	// std::array<int, (size_t)ComponentType::NUM_COMPONENTS> compIndices{{EMPTY_INDEX}};
	int compIndices[6] = {-1, -1, -1, -1, -1, -1};
};

namespace GO
{
    bool hasComponent(GameObject* gameOjbject, ComponentType type);
}

#endif
