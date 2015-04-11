#include "scenemanager.h"
#include "gameobject.h"
#include "transform.h"
#include "scriptengine.h"
#include "passert.h"

namespace SceneManager
{
	namespace
	{
		std::vector<Node>         validNodes;
		std::vector<Node>         removableNodes;
		std::vector<GameObject>   sceneObjects;
		std::vector<unsigned int> emptyIndices;
		
		void removeGameObject(GameObject* gameObject)
		{
			//remove components
			for(int i = 1; i < (int)Component::NUM_COMPONENTS; i++)
			{
				if(gameObject->compIndices[i] != -1)
					GO::removeComponent(gameObject, (Component)i);
			}
			//remove scripts
			ScriptEngine::unRegisterGameObject(gameObject);
			
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
		GameObject* gameObject = find(node);
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
		GameObject* gameObject = find(name);
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
	
	GameObject* find(const std::string& name)
	{
		GameObject* gameObject = NULL;
		for(Node node : validNodes)
		{
			GameObject* temp = &sceneObjects[node];
			if(name == temp->name)
			{
				gameObject = temp;
				break;
			}
		}
		if(!gameObject) Log::warning("Gameobject '" + name + "' not found in scene");
		return gameObject;
	}

	GameObject* find(Node nodeToFind)
	{
		GameObject* gameObject = NULL;		
		for(Node node : validNodes)
		{
			if(node == nodeToFind)
			{
				gameObject = &sceneObjects[node];
				break;
			}
		}
		if(!gameObject) Log::warning("GO " + std::to_string(nodeToFind) + " not found in scene");
		return gameObject;
	}

	void update()
	{
		//Remove Marked GOs, if any
		if(removableNodes.size() > 0)
		{
			for(Node node : removableNodes)
		    {
				GameObject* gameObject = find(node);
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

	GameObject* create(const std::string& name)
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
		
		GameObject* newObj = &sceneObjects[index];
		newObj->name = name;
		newObj->node = index;
		GO::addTransform(newObj);
		ScriptEngine::registerGameObject(newObj);
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

	GameObject* findByName(const std::string& name)
	{
		return find(name);
	}
	
	GameObject* findByNode(Node node)
	{
		return find(node);
	}

	void generateBindings()
	{
		asIScriptEngine* engine = ScriptEngine::getEngine();
		engine->SetDefaultNamespace("SceneManager");
		int rc = -1;
		rc = engine->RegisterGlobalFunction("void remove(const string)",
											asFUNCTIONPR(remove, (const std::string&), bool),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("void remove(int32)",
											asFUNCTIONPR(remove, (Node), bool),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("GameObject@ find(const string)",
											asFUNCTIONPR(find, (const std::string&), GameObject*),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("GameObject@ find(int32)",
											asFUNCTIONPR(find, (Node), GameObject*),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("GameObject@ create(const string)",
											asFUNCTION(create),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->SetDefaultNamespace("");
	}
}
