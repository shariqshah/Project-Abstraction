#include "scenemanager.h"
#include "gameobject.h"
#include "transform.h"
#include "scriptengine.h"
#include "passert.h"
#include "utilities.h"
#include "light.h"
#include "camera.h"
#include "model.h"

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

	unsigned int createNewIndex()
	{
		unsigned int index = 0;
		if(!emptyIndices.empty())
		{
			index = emptyIndices.back();
			emptyIndices.pop_back();
			validNodes.push_back(index);
		}
		else
		{
			sceneObjects.push_back(GameObject());
			index = sceneObjects.size() - 1;
			validNodes.push_back(index);
		}
		return index;
	}

	GameObject* createFromFile(const std::string& filename)
	{
		using namespace rapidjson;
		bool success = true;
		const char* error = "NONE";
	    GameObject* gameobject = NULL;
		Document document;
		char* json = Utils::loadFileIntoCString(filename.c_str());
		if(json)
		{
			document.Parse(json);
			if(!document.IsObject() || !document.HasMember("GameObject"))
			{
				error = "Invalid file";
				success = false;
			}
			else
			{
				Value& gameobjectNode = document["GameObject"];
				if(gameobjectNode.IsObject())
				{
					if(gameobjectNode.HasMember("Name") && gameobjectNode["Name"].IsString())
					{
						Value& name = gameobjectNode["Name"];
						int index = createNewIndex();
						sceneObjects[index] = GameObject();
						gameobject = &sceneObjects[index];
						gameobject->node = index;
						gameobject->name = name.GetString();
						if(gameobjectNode.HasMember("Tag") && gameobjectNode["Tag"].IsString())
							gameobject->tag = gameobjectNode["Tag"].GetString();
						GO::addTransform(gameobject);
						ScriptEngine::registerGameObject(gameobject);
						Log::message(gameobject->name + " added to scene");
						if(gameobjectNode.HasMember("Components"))
						{
							Value& componentNode = gameobjectNode["Components"];
							if(componentNode.IsObject())
							{
								if(componentNode.HasMember("Transform"))
								{
									CTransform* transform = GO::getTransform(gameobject);
									if(!Transform::createFromJSON(transform, componentNode["Transform"]))
										Log::warning("Errors while initializing Transform from " + filename);
								}

								if(componentNode.HasMember("Light"))
								{
									CLight* light = GO::addLight(gameobject);
									if(!Light::createFromJSON(light, componentNode["Light"]))
										Log::warning("Errors while initializing Light from " + filename);
								}

								if(componentNode.HasMember("Camera"))
								{
									CCamera* camera = GO::addCamera(gameobject);
									if(!Camera::createFromJSON(camera, componentNode["Camera"]))
										Log::warning("Errors while initializing Camera from " + filename);
								}

								if(componentNode.HasMember("Model"))
								{
									CModel* model = GO::addModel(gameobject, "default.pamesh");
									if(!Model::createFromJSON(model, componentNode["Model"]))
										Log::warning("Errors while initializing Model from " + filename);
								}
							}
							else
							{
								Log::warning("'Components' is not an object. No components added to " + gameobject->name);
							}
						}

						if(gameobjectNode.HasMember("Scripts"))
						{
							Value& scripts = gameobjectNode["Scripts"];
							if(scripts.IsArray())
							{
								for(SizeType i = 0; i < scripts.Size(); i++)
								{
									if(scripts[i].IsString())
										ScriptEngine::addScript(gameobject, scripts[i].GetString());
									else
										Log::warning("Invalid script name in " + filename);
								}
							}
							else if(scripts.IsString())
							{
								ScriptEngine::addScript(gameobject, scripts.GetString());
							}
							else
							{
								Log::warning("Invalid value for 'Scripts' in " + filename);
							}
						}
					}
					else
					{
						error = "Gameobject does not have a name";
						success = false;
					}
				}
			}
			free(json);
		}
		if(!success) Log::error("SceneManager::createFromFile", error);
		return gameobject;
	}

	GameObject* create(const std::string& name)
	{
		GameObject* newObj = NULL;
		if(name.size() > 0)
		{
			unsigned int index = createNewIndex();
			newObj = &sceneObjects[index];
			newObj->name = name;
			newObj->node = index;
			GO::addTransform(newObj);
			ScriptEngine::registerGameObject(newObj);
			Log::message(name + " added to scene");
		}
		else
		{
			Log::error("SceneManager::create", "Invalid name");
		}
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
		rc = engine->RegisterGlobalFunction("GameObject@ createFromFile(const string)",
											asFUNCTION(createFromFile),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->SetDefaultNamespace("");
	}
}
