#include "scenemanager.h"

namespace SceneManager
{
	namespace
	{
		static SceneObjectMap sSceneObjects;
		static std::vector<Node> sRemovables;
	}
	
	bool add(GOPtr newGameObject)
	{
		Node node = newGameObject->getNode();
		const std::string name = newGameObject->getName();
		
		std::pair<SceneObjectMap::iterator, bool> returnValue;
		returnValue = sSceneObjects.insert(std::make_pair(node, newGameObject));

		if(returnValue.second)
		{
			Log::message(name + " added to scene");
			return true;
		}
		else
		{
			Log::error(Log::ErrorLevel::MEDIUM, name + " could not be added to scene");
			return false;
		}
	}
	
	bool remove(std::string name)
	{
		for(SceneObjectMap::iterator it = sSceneObjects.begin();
			it != sSceneObjects.end();
			++it)
		{
			if(name == it->second->getName())
			{
				sRemovables.push_back(it->first);
				return true;
			}
		}

		Log::error(Log::ErrorLevel::LOW,
				   name + " not found in scene so cannot be removed.");
		return false;
	}
	
	GOPtr find(std::string name)
	{
		for(SceneObjectMap::iterator it = sSceneObjects.begin();
			it != sSceneObjects.end();
			++it)
		{
			if(name == it->second->getName())
				return it->second;
		}

		return nullptr;
	}

	void update()
	{
		//Remove Marked GOs
		for(Node node : sRemovables)
		{
			int removed = sSceneObjects.erase(node);
			if(removed == 0)
				Log::warning("GO marked for removal could not be removed!");
		}

		sRemovables.clear();
	}

	void cleanup()
	{
		std::cout<<"Size before : "<<sSceneObjects.size()<<std::endl;
		sSceneObjects.clear();
		std::cout<<"Size after : "<<sSceneObjects.size()<<std::endl;

		if(!sRemovables.empty())
			sRemovables.clear();
	}

	GOPtr createGameObject(const std::string name)
	{
		GOPtr newObj = std::make_shared<GameObject>(name);
		newObj->addComponent(std::make_shared<Transform>());
		add(newObj);
		
		return newObj;
	}

	SceneObjectMap* getSceneObjects()
	{
		return &sSceneObjects;
	}
}
