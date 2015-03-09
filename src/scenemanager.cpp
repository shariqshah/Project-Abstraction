#include "scenemanager.h"
#include "gameobject.h"
#include "componentmanager.h"
#include "transform.h"
#include "scriptengine.h"

namespace SceneManager
{
	namespace
	{
		std::vector<Node>         validNodes;
		std::vector<Node>         removableNodes;
		std::vector<GameObject>   sceneObjects;
		std::vector<unsigned int> emptyIndices;
		
		void removeGameObject(GOPtr gameObject)
		{
			//remove components
			for(int i = 1; i < (int)Component::NUM_COMPONENTS; i++)
			{
				if(gameObject->compIndices[i] != -1)
					GO::removeComponent(gameObject, (Component)i);
			}

			//remove scripts
			ScriptEngine::executeFunction("removeGameObject", gameObject);

			// remove the node from valid list and mark it's location in list as empty
			int index = -1;
			for(size_t i = 0; i < validNodes.size(); i++)
			{
				if(validNodes[i] == gameObject->node)
				{
					index = (int)i;
					emptyIndices.push_back(index);
					break;
				}
			}

			if(index != -1)
				validNodes.erase(validNodes.begin() + index);
			else
				Log::error("SceneManager::removeGameObject", "Could not remove node");
		}

		bool markForDeletion(Node nodeToMark)
		{
			// Check for doubles
			for(Node node : removableNodes)
			{
				if(node == nodeToMark)
					return false;
			}
			removableNodes.push_back(nodeToMark);
			return true;
		}
	}
	
	bool remove(Node node)
	{
		GOPtr gameObject = find(node);
		if(gameObject)
		{
			if(markForDeletion(node))
				Log::message(gameObject->name + " marked for removal");
			else
				Log::message(gameObject->name + " already marked for removal");
			
			return true;
		}
		Log::error("SceneManager", "GO " + std::to_string(node) +
				   " not found in scene so cannot be removed.");
		return false;
	}
	
	bool remove(const std::string& name)
	{
		GOPtr gameObject = find(name);
		if(gameObject)
		{
			if(markForDeletion(gameObject->node))
				Log::message(name + " marked for removal");
			else
				Log::message(name + " already marked for removal");
			
			return true;
		}		

		Log::error("SceneManager::remove",
				   name + " not found in scene so cannot be removed.");
		return false;
	}
	
	GOPtr find(const std::string& name)
	{
		GOPtr gameObject = NULL;
		bool  found      = false;

		for(Node node : validNodes)
		{
			gameObject = &sceneObjects[node];
			if(name == gameObject->name)
			{
				found = true;
				break;
			}
		}
		if(!found)
			Log::warning(name + " not found in scene");
		
		return gameObject;
	}

	GOPtr find(Node nodeToFind)
	{
		GOPtr gameObject = NULL;		
		for(Node node : validNodes)
		{
			if(node == nodeToFind)
			{
				gameObject = &sceneObjects[node];
				break;
			}
		}
		if(!gameObject)
			Log::warning("GO " + std::to_string(nodeToFind) + " not found in scene");
		
		return gameObject;
	}

	void update()
	{
		//Remove Marked GOs, if any
		if(removableNodes.size() > 0)
		{
			for(Node node : removableNodes)
		    {
				GOPtr gameObject = find(node);
				removeGameObject(gameObject);
			}			
			removableNodes.clear();
		}
	}

	void cleanup()
	{
		for(Node node : validNodes)
			markForDeletion(node);
		
		update();
		sceneObjects.clear();
		removableNodes.clear();
	}

	GOPtr create(const std::string& name)
	{
		unsigned int index = 0;
		if(!emptyIndices.empty())
		{
			index = emptyIndices.back();
			emptyIndices.pop_back();
			validNodes.push_back(index);
			sceneObjects[index] = GameObject();
		}
		else
		{
			sceneObjects.push_back(GameObject());
			index = sceneObjects.size() - 1;
			validNodes.push_back(index);
		}
		
		GOPtr newObj = &sceneObjects[index];
		newObj->name = name;
		newObj->node = index;
		GO::addTransform(newObj);
		Log::message(name + " added to scene");
		
		return newObj;
	}

	std::vector<Node>* getSceneObjects()
	{
		return &validNodes;
	}

	bool removeByName(const std::string& name)
	{
		return remove(name);
	}

	bool removeByNode(Node node)
	{
		return remove(node);
	}

	GOPtr findByName(const std::string& name)
	{
		return find(name);
	}
	
	GOPtr findByNode(Node node)
	{
		return find(node);
	}

	void generateBindings()
	{
		Sqrat::RootTable().Bind("SceneManager", Sqrat::Table(ScriptEngine::getVM())
								.Func("removeByNode",    &removeByNode)
								.Func("removeByName",    &removeByName)
								.Func("findByNode",      &findByNode)
								.Func("findByName",      &findByName)
								.Func("create",          &create));
	}
}
