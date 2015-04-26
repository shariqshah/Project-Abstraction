#include "scenemanager.h"
#include "gameobject.h"
#include "transform.h"
#include "scriptengine.h"
#include "passert.h"
#include "utilities.h"
#include "light.h"
#include "rigidbody.h"
#include "camera.h"
#include "model.h"
#include "renderer.h"
#include "physics.h"

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
		validNodes.clear();
		emptyIndices.clear();
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

	bool writeToJSON(GameObject* gameobject, rapidjson::Writer<rapidjson::StringBuffer>& writer)
	{
		PA_ASSERT(gameobject);
		using namespace rapidjson;
		bool success = true;
		
		writer.StartObject();
		writer.Key("Name");	writer.String(gameobject->name.c_str(), gameobject->name.size());
		writer.Key("Tag");  writer.String(gameobject->tag.c_str(), gameobject->tag.size());
		// Scripts
		int scriptCount = ScriptEngine::getAttachedScriptsCount(gameobject);
		if(scriptCount > 0)
		{
			writer.Key("Scripts");
			writer.StartArray();
			for(int i = 0; i < scriptCount; i++)
				writer.String(ScriptEngine::getAttachedScriptName(gameobject, i).c_str());
			writer.EndArray();
		}
		writer.Key("Components");
		writer.StartObject();
		// Transform
		CTransform* transform = GO::getTransform(gameobject);
		if(!Transform::writeToJSON(transform, writer))
		{
		 	success = false;
			Log::error("SceneManager::writeToJSON", "Problem writing transform for " + gameobject->name);
		}
		// Model
		if(GO::hasComponent(gameobject, Component::MODEL))
		{
			CModel* model = GO::getModel(gameobject);
			if(!Model::writeToJSON(model, writer))
			{
				success = false;
				Log::error("SceneManager::writeToJSON", "Problem writing model for " + gameobject->name);
			}
		}
		// Camera
		if(GO::hasComponent(gameobject, Component::CAMERA))
		{
			CCamera* camera = GO::getCamera(gameobject);
			if(!Camera::writeToJSON(camera, writer))
			{
				success = false;
				Log::error("SceneManager::writeToJSON", "Problem writing camera for " + gameobject->name);
			}
		}
		// Light
		if(GO::hasComponent(gameobject, Component::LIGHT))
		{
			CLight* light = GO::getLight(gameobject);
			if(!Light::writeToJSON(light, writer))
			{
				success = false;
				Log::error("SceneManager::writeToJSON", "Problem writing light for " + gameobject->name);
			}
		}
		// RigidBody
		if(GO::hasComponent(gameobject, Component::RIGIDBODY))
		{
			CRigidBody rigidBody = GO::getRigidBody(gameobject);
			if(!RigidBody::writeToJSON(rigidBody, writer))
			{
				success = false;
				Log::error("SceneManager::writeToJSON", "Problem writing rigidBody for " + gameobject->name);
			}
		}
		writer.EndObject();
		writer.EndObject();
		return success;
	}

	bool writeSceneToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer)
	{
		using namespace rapidjson;
		bool success = true;
		writer.StartObject();
		// Scene
		writer.Key("Scene"); writer.StartObject();
		{
			writer.Key("SceneObjects");
			writer.StartArray();
			for(int i = 0; i < (int)validNodes.size(); i++)	writeToJSON(&sceneObjects[validNodes[i]], writer);
			writer.EndArray();
			// Renderer
			RenderParams* renderParams = Renderer::getRenderParams();
			writer.Key("Renderer"); writer.StartObject();
			{
				writer.Key("AmbientLight");
				writer.StartArray();
				for(int i = 0; i < 4; i++) writer.Double(renderParams->ambientLight[i]);
				writer.EndArray();
				writer.Key("ClearColor");
				writer.StartArray();
				Vec4 clearColor = Renderer::getClearColor();
				for(int i = 0; i < 4; i++) writer.Double(clearColor[i]);
				writer.EndArray();
				writer.Key("Fog"); writer.StartObject();
				{
					writer.Key("FogMode"); writer.Int(renderParams->fog.fogMode);
					writer.Key("Density"); writer.Double(renderParams->fog.density);
					writer.Key("Start");   writer.Double(renderParams->fog.start);
					writer.Key("Max");     writer.Double(renderParams->fog.max);
					writer.Key("Color");
					writer.StartArray();
					for(int i = 0; i < 4; i++) writer.Double(renderParams->fog.color[i]);
					writer.EndArray();
				}
				writer.EndObject();
			}
			writer.EndObject();
			writer.Key("Physics"); writer.StartObject();
			{
				writer.Key("Gravity");
				writer.StartArray();
				Vec3 gravity = Physics::getGravity();
				for(int i = 0; i < 3; i++) writer.Double(gravity[i]);
				writer.EndArray();
			}
			writer.EndObject();
		}
		writer.EndObject();
		writer.EndObject();
		return success;
	}

	bool saveScene(const std::string& filename)
	{
		bool success = true;
		using namespace rapidjson;
		StringBuffer buffer;
		PrettyWriter<StringBuffer> writer(buffer);
		writer.SetIndent('\t', 1);
		success = writeSceneToJSON(writer);

		FILE* newFile = fopen(filename.c_str(), "w+");
		if(newFile)
		{
			size_t rc = fwrite((void*)buffer.GetString(), buffer.GetSize(), 1, newFile);
			if(rc <= 0)
			{
				success = false;
				Log::error("SceneManager::saveScene", "Error writing file");
			}
			else
			{
				Log::message("Scene saved to '" + filename + "'");
			}
			fclose(newFile);
		}
		else
		{
			success = false;
			Log::error("SceneManager::saveScene", "File could not be created");
		}
		
		return success;
	}

	bool saveGameObject(GameObject* gameobject,const std::string& filename)
	{
		PA_ASSERT(gameobject);
		using namespace rapidjson;
		bool success = true;
		StringBuffer buffer;
		PrettyWriter<StringBuffer> writer(buffer);
		writer.SetIndent('\t', 1);
		success = writeToJSON(gameobject, writer);
		
		FILE* newFile = fopen(filename.c_str(), "w+");
		if(newFile)
		{
			size_t rc = fwrite((void*)buffer.GetString(), buffer.GetSize(), 1, newFile);
			if(rc <= 0)
			{
				success = false;
				Log::error("SceneManager::saveGameObject", "Error writing file");
			}
			else
			{
				Log::message(gameobject->name + " saved to '" + filename + "'");
			}
			fclose(newFile);
		}
		else
		{
			success = false;
			Log::error("SceneManager::saveGameObject", "File could not be created");
		}
		return success;
	}

	GameObject* createFromJSON(const rapidjson::Value& gameobjectNode, const std::string& filename)
	{
		using namespace rapidjson;
		bool success = true;
		const char* error = "NONE";
	    GameObject* gameobject = NULL;
		if(gameobjectNode.IsObject())
		{
			if(gameobjectNode.HasMember("Name") && gameobjectNode["Name"].IsString())
			{
				const Value& name = gameobjectNode["Name"];
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
					const Value& componentNode = gameobjectNode["Components"];
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

						if(componentNode.HasMember("RigidBody"))
						{
							CRigidBody rigidbody = GO::addRigidbody(gameobject, NULL);
							if(!RigidBody::createFromJSON(rigidbody, componentNode["RigidBody"]))
								Log::warning("Errors while initializing Rigidbody from " + filename);
						}
					}
					else
					{
						Log::warning("'Components' is not an object. No components added to " + gameobject->name);
					}
				}

				if(gameobjectNode.HasMember("Scripts"))
				{
					const Value& scripts = gameobjectNode["Scripts"];
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
				success = false;
				error   = "Gameobject does not have a name";
			}
		}
		else
		{
			success = false;
			error   = "'GameObject' should be an object";
		}
		
		if(!success) Log::error("SceneManager::createFromFile", error);
		return gameobject;
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
			//if(document.IsObject() && document.HasMember("GameObject") && document["GameObject"].IsObject())
			if(document.IsObject())
			{
				gameobject = createFromJSON(document, filename);
			}
			else
			{
				success = false;
				error   = "Invalid document";
			}
			free(json);
		}
		else
		{
			success = false;
			error   = "File not found";
		}
		if(!success) Log::error("SceneManager::createFromFile", error);
		return gameobject;
	}

	bool loadScene(const std::string& filename)
	{
		using namespace rapidjson;
		bool success = true;
		const char* error = "Invalid field";
		char* json = Utils::loadFileIntoCString(filename.c_str());
		if(json)
		{
			Document document;
			document.Parse(json);
			if(document.IsObject() && document.HasMember("Scene") && document["Scene"].IsObject())
			{
				const Value& sceneNode = document["Scene"];
				// Load all gameobjects
				if(sceneNode.HasMember("SceneObjects") && sceneNode["SceneObjects"].IsArray())
				{
					const Value& sceneObjects = sceneNode["SceneObjects"];
					int loaded = 0;
					for(unsigned int i = 0; i < sceneObjects.Size(); i++)
					{
						if(sceneObjects[i].IsObject())
						{
							if(createFromJSON(sceneObjects[i], filename))
								loaded++;
						}
						else
						{
							success = false;
						}
					}
					Log::message(std::to_string(loaded) + " gameobjects loaded from " + filename);
				}

				// Set Renderer settings
				if(sceneNode.HasMember("Renderer") && sceneNode["Renderer"].IsObject())
				{
					RenderParams* renderParams = Renderer::getRenderParams();
					const Value&  rendererNode = sceneNode["Renderer"];

					if(rendererNode.HasMember("AmbientLight") && rendererNode["AmbientLight"].IsArray())
					{
						const Value& colorNode = rendererNode["AmbientLight"];
						int items = colorNode.Size() < 4 ? colorNode.Size() : 4;
						for(int i = 0; i < items; i++)
						{
							if(colorNode[i].IsNumber())
								renderParams->ambientLight[i] = (float)colorNode[i].GetDouble();
							else
								success = false;
						}
					}
					else
					{
						success = false;
						Log::error("SceneManger::loadScene", "Error while loading Renderer.AmbientLight");
					}

					if(rendererNode.HasMember("ClearColor") && rendererNode["ClearColor"].IsArray())
					{
						const Value& clearColorNode = rendererNode["ClearColor"];
						int items = clearColorNode.Size() < 4 ? clearColorNode.Size() : 4;
						Vec4 clearColor = Renderer::getClearColor();
						for(int i = 0; i < items; i++)
						{
							if(clearColorNode[i].IsNumber())
								clearColor[i] = (float)clearColorNode[i].GetDouble();
							else
								success = false;
						}
						Renderer::setClearColor(clearColor);
					}
					else
					{
						success = false;
						Log::error("SceneManger::loadScene", "Error while loading Renderer.ClearColor");
					}
					
					if(rendererNode.HasMember("Fog") && rendererNode["Fog"].IsObject())
					{
						const Value& fogNode = rendererNode["Fog"];
						if(fogNode.HasMember("FogMode") && fogNode["FogMode"].IsInt())
						{
							int mode = fogNode["FogMode"].GetInt();
							if(mode > -1 && mode < 4)
								renderParams->fog.fogMode = mode;
							else
								success = false;
						}
						else
						{
							success = false;
							Log::error("SceneManger::loadScene", "Error while loading Fog.FogMode");
						}

						if(fogNode.HasMember("Density") && fogNode["Density"].IsDouble())
						{
							float density = (float)fogNode["Density"].GetDouble();
							if(density >= 0.f)
								renderParams->fog.density = density;
							else
								success = false;
						}
						else
						{
							success = false;
							Log::error("SceneManger::loadScene", "Error while loading Fog.Density");
						}

						if(fogNode.HasMember("Start") && fogNode["Start"].IsDouble())
						{
							float start = (float)fogNode["Start"].GetDouble();
							if(start >= 0)
								renderParams->fog.start = start;
							else
								success = false;
						}
						else
						{
							success = false;
							Log::error("SceneManger::loadScene", "Error while loading Fog.Start");
						}

						if(fogNode.HasMember("Max") && fogNode["Max"].IsDouble())
						{
							float max = (float)fogNode["Max"].GetDouble();
							if(max >= 0)
								renderParams->fog.max = max;
							else
								success = false;
						}
						else
						{
							success = false;
							Log::error("SceneManger::loadScene", "Error while loading Fog.max");
						}

						if(fogNode.HasMember("Color") && fogNode["Color"].IsArray())
						{
							const Value& colorNode = fogNode["Color"];
							int items = colorNode.Size() < 4 ? colorNode.Size() : 4;
							for(int i = 0; i < items; i++)
							{
								if(colorNode[i].IsNumber())
									renderParams->fog.color[i] = (float)colorNode[i].GetDouble();
								else
									success = false;
							}
						}
						else
						{
							success = false;
							Log::error("SceneManger::loadScene", "Error while loading Fog.Color");
						}
					}
				}
				else
				{
					success = false;
					Log::error("SceneManger::loadScene", "Error while loading Renderer");
				}
				
				// Physics settings
				if(sceneNode.HasMember("Physics") && sceneNode["Physics"].IsObject())
				{
					const Value& physicsNode = sceneNode["Physics"];
					if(physicsNode.HasMember("Gravity") && physicsNode["Gravity"].IsArray())
					{
						const Value& gravityNode = physicsNode["Gravity"];
						int items = gravityNode.Size() < 3 ? gravityNode.Size() : 3;
						Vec3 gravity = Physics::getGravity();
						for(int i = 0; i < items; i++)
						{
							if(gravityNode[i].IsNumber())
								gravity[i] = (float)gravityNode[i].GetDouble();
							else
								success = false;
						}
						Physics::setGravity(gravity);
					}
					else
					{
						success = false;
						Log::error("SceneManger::loadScene", "Error while loading gravity");
					}
				}
				else
				{
					success = false;
					Log::error("SceneManger::loadScene", "Error while loading Physics settings");
				}
			}
			else
			{
				success = false;
				error   = "Invalid document format";
			}
			free(json);
		}
		else
		{
			success = false;
			error   = "File not found";
		}
	
		if(!success) Log::error("SceneManager::loadScene", error);
		return success;
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
		rc = engine->RegisterGlobalFunction("bool loadScene(const string &in)",
											asFUNCTION(loadScene),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("bool saveGameObject(GameObject@, const string &in)",
											asFUNCTION(saveGameObject),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("bool saveScene(const string &in)",
											asFUNCTION(saveScene),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->SetDefaultNamespace("");
	}
}
