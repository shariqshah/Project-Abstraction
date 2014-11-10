#include "gameobject.h"

namespace GO
{
    bool hasComponent(GameObject* gameObject, ComponentType type)
	{
		// std::cout<<"Type : "<<(int)type<<std::endl;
		// std::cout<<"Index : "<<gameObject->compIndices[(int)type]<<std::endl;
		
		if(gameObject->compIndices[(int)type] == -1)
			return false;
		else
			return true;
	}
}
