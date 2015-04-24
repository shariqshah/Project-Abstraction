#include "scriptengine.h"
#include <vector>
#include "log.h"
#include "gameobject.h"
#include "scenemanager.h"
#include "utilities.h"
#include "datatypes.h"
#include "../include/angelscript/add_on/scriptstdstring/scriptstdstring.h"
#include "../include/angelscript/add_on/scriptbuilder/scriptbuilder.h"
#include "../include/angelscript/add_on/scripthelper/scripthelper.h"
#include "passert.h"

struct Script
{
	std::string        module;
	bool               enabled       = false;
	asIScriptFunction* updateFunc    = NULL;
	asIScriptFunction* initFunc      = NULL;
	asIScriptFunction* collisionFunc = NULL;
	asIObjectType*     objType       = NULL;
	asIScriptObject*   scriptObj     = NULL;
};

struct ScriptContainer
{
	std::vector<Script> scripts;
	Node gameObjectNode;
};

namespace ScriptEngine
{
	bool reloadQueuedScript(const std::string& scriptName);
	
	namespace
	{
		asIScriptEngine*  engine = NULL;
		// asIScriptContext* context = NULL;
		std::vector<asIScriptContext*> contextPool;
		std::vector<ScriptContainer> scriptContainerList;
		std::vector<int> scriptContainerEmptyIndices;
		std::vector<int> activeScriptContainers;
		std::string scriptDir = "../content/scripts/"; // TODO: Find a better way to do this
		std::vector<std::string> scriptReloadQueue;
		
		int execute(asIScriptContext* context)
		{
			int rc = context->Execute();
			if(rc == asEXECUTION_EXCEPTION)
			{
				printf("Exception : %s \nFunction : %s \nLine : %d\n",
					   context->GetExceptionString(),
					   context->GetExceptionFunction()->GetDeclaration(),
					   context->GetExceptionLineNumber());
			}
			return rc;
		}

		int findScriptLocation(ScriptContainer* container, const std::string& scriptName)
		{
			int scriptLocation = -1;
			for(int i = 0; i < (int)container->scripts.size(); i++)
			{
				Script& script = container->scripts[i];
				if(script.module == scriptName)
				{
					scriptLocation = i;
					break;
				}
			}
			return scriptLocation;
		}
	}

	asIScriptContext* prepareContext(asIScriptFunction* function)
	{
		asIScriptContext* context = NULL;
		if(contextPool.size() > 0)
		{
			context = contextPool.back();
			contextPool.pop_back();
		}
		else
		{
			context = engine->CreateContext();
		}
		int rc = context->Prepare(function); PA_ASSERT(rc >= 0);
		return context;
	}

	void returnContext(asIScriptContext* context)
	{
		contextPool.push_back(context);
		context->Unprepare();
	}

	void MessageCallback(const asSMessageInfo *msg, void *param)
	{
		std::string type = "ERROR";
		if( msg->type == asMSGTYPE_WARNING ) 
			type = "WARNING";
		else if( msg->type == asMSGTYPE_INFORMATION ) 
			type = "INFO";

		std::string message = "";
		message += msg->section;
		message += "(";
		message += std::to_string(msg->row);
		message += ", ";
		message += std::to_string(msg->col);
		message += ") : ";
		message += type;
		message += " : ";
		message += msg->message;
		if(type == "INFO")
			Log::message("AngelScript : " +  message);
		else if(type == "WARNING")
			Log::warning("AngelScript : " + message);
		else
			Log::error("Angelscript", message);
	}

	
	bool initialize()
	{
		bool success = true;		
		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		if(engine == 0)
		{
			Log::error("ScriptEngine::initialize", "Failed to create engine");
			success = false;
		}
		else
		{
			// The script compiler will write any compiler messages to the callback.
			engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
			// context = engine->CreateContext();
			RegisterStdString(engine); // Register string type
			// Bind functions for script reloading
			engine->SetDefaultNamespace("ScriptEngine");
			int rc = engine->RegisterGlobalFunction("void reloadScript(const string &in)",
													asFUNCTION(reloadScript),
													asCALL_CDECL);
			PA_ASSERT(rc >= 0);
			engine->SetDefaultNamespace("");
		}
		return success;
	}

	void registerScriptInterface()
	{
		// This function is called later because at initialization of scriptengine CollisionData
		// isn't registered yet so this function is called after all other types have been registered
		int rc = engine->RegisterInterface("IScriptable"); PA_ASSERT(rc >= 0);
		engine->RegisterInterfaceMethod("IScriptable", "void update(float)"); PA_ASSERT(rc >= 0);
		engine->RegisterInterfaceMethod("IScriptable", "void onCollision(const CollisionData@)");
		PA_ASSERT(rc >= 0);
	}

	void cleanup()
	{
		if(engine)
		{
			for(unsigned int i = 0; i < contextPool.size(); i++)
				contextPool[i]->Release();
			engine->ShutDownAndRelease();
		}
	}
	
	asIScriptEngine* getEngine()
	{
		return engine;
	}

	void registerGameObject(GameObject* gameObject)
	{
		PA_ASSERT(gameObject);
		bool found = false;
		// Check that gameObject is not already registered
		for(int activeContainerIndex : activeScriptContainers)
		{
			ScriptContainer& container = scriptContainerList[activeContainerIndex];
			if(container.gameObjectNode == gameObject->node)
			{
				found = true;
				break;
			}
		}

		if(found) // No need to register, gameobject is already registered
		{
			Log::warning(gameObject->name + " already registered with script engine");
			return;
		}

		int index = -1;
		if(scriptContainerEmptyIndices.empty())
		{
			scriptContainerList.push_back(ScriptContainer());
			index = (int)(scriptContainerList.size() - 1);
		}
		else
		{
			index = scriptContainerEmptyIndices.back();
			scriptContainerEmptyIndices.pop_back();
		}
		scriptContainerList[index].gameObjectNode = gameObject->node;
		activeScriptContainers.push_back(index);
		gameObject->scriptIndex = index;
		Log::message(gameObject->name + " registered with scripting engine");
	}

	bool createModule(const std::string& scriptName, bool appendTemp = false)
	{
		bool success = true;
		CScriptBuilder builder;
		// If appendTemp is true then append TEMP to name to avoid collision with existing module
		std::string moduleName = appendTemp ? scriptName + "TEMP" : scriptName;
		int rc = builder.StartNewModule(engine, moduleName.c_str());
		std::string scriptPath(scriptDir + scriptName + ".as");
		if(Utils::fileExists(scriptPath.c_str()))
		{
			rc = builder.AddSectionFromFile(scriptPath.c_str());
			if(rc < 0)
			{
				Log::error("ScriptEngine::createModule", "Error compiling " + scriptName);
				success = false;
			}
			else
			{
				rc = builder.BuildModule();
				if(rc < 0)
				{
					Log::error("ScriptEngine::createModule", "Error building module " + moduleName);
					success = false;
				}
			}
		}
		else
		{
			Log::error("ScriptEngine::createModule", "Script '" + scriptName + "' not found");
		}
		return success;
	}
	
	void addScript(GameObject* gameObject, const std::string& scriptName)
	{
		PA_ASSERT(gameObject);
		ScriptContainer* container = &scriptContainerList[gameObject->scriptIndex];
		// Check if module has already been created
		asIScriptModule* module = engine->GetModule(scriptName.c_str(), asGM_ONLY_IF_EXISTS);
		if(!module)
		{
			bool moduleCreated = createModule(scriptName);
			if(moduleCreated)
			{
				module = engine->GetModule(scriptName.c_str(), asGM_ONLY_IF_EXISTS);
			}
			else
			{
				Log::error("ScriptEngine::addScript", scriptName + " not added to " + gameObject->name);
				return;
			}
		}
		container->scripts.push_back(Script());
		Script& newScript = container->scripts[container->scripts.size() - 1];
		newScript.module = scriptName;
		newScript.enabled = true;
		
		// Find object type that implements IScriptable interface
		int objectCount = module->GetObjectTypeCount();
		asIObjectType* type = NULL;
		for(int i = 0; i < objectCount; i++)
		{
			bool found = false;
			type = module->GetObjectTypeByIndex(i);
			int interfaceCount = type->GetInterfaceCount();
			for(int j = 0; j < interfaceCount; j++)
			{
				if(strcmp(type->GetInterface(i)->GetName(), "IScriptable") == 0)
				{
					found = true;
					break;
				}
			}
			if( found == true )
			{
				newScript.objType = type;
				break;
			}
		}
		if(type == NULL)
		{
			Log::error("ScriptEngine::addScript",
					   "No class implmenting IScriptable interface found in " + scriptName);
			Log::error("ScriptEngine::addScript", scriptName + " not added to " + gameObject->name);
			container->scripts.pop_back();
			return;
		}

		// Find the update and init functions in the interface
		std::string typeName = std::string(type->GetName());
		std::string initFuncDecl = typeName + "@ " + typeName + "(int32)";
		std::string updateFuncDecl = "void update(float)";
		std::string onCollFuncDecl = "void onCollision(const CollisionData@)";
		newScript.initFunc = type->GetFactoryByDecl(initFuncDecl.c_str());
		newScript.updateFunc = type->GetMethodByDecl(updateFuncDecl.c_str());
		newScript.collisionFunc = type->GetMethodByDecl(onCollFuncDecl.c_str());

		// Create the script object
		asIScriptContext* context = prepareContext(newScript.initFunc);
		context->SetArgDWord(0, gameObject->node);
		int rc = execute(context);
		if(rc == asEXECUTION_FINISHED)
		{
			// Get the newly created scriptobject and increase it's reference count
			newScript.scriptObj = *((asIScriptObject**)context->GetAddressOfReturnValue());
			newScript.scriptObj->AddRef();
			Log::message("Script '" + scriptName + "' added to " + gameObject->name);
		}
		else if(rc == asEXECUTION_EXCEPTION)
		{
			Log::error("ScriptEngine::addScript", scriptName + " not added to " + gameObject->name);
			container->scripts.pop_back();
		}
		returnContext(context);
	}

	void updateAllScripts(float deltaTime)
	{
		//for(int activeContainer : activeScriptContainers)
		for(int i = 0; i < (int)activeScriptContainers.size(); i++)
		{
			ScriptContainer* container = &scriptContainerList[i];
			for(Script& script : container->scripts)
			{
				if(script.enabled)
				{
					asIScriptContext* context = prepareContext(script.updateFunc);
					context->SetObject(script.scriptObj);
					context->SetArgFloat(0, deltaTime);
					execute(context);
					returnContext(context);
				}
			}
		}
		// If any scripts are queued to be reloaded, reload them
		for(const std::string& queuedScript : scriptReloadQueue) reloadQueuedScript(queuedScript);
		scriptReloadQueue.clear();
	}

	void callCollisionCallcallbacks(GameObject* gameObject, const CollisionData* collisionData)
	{
		PA_ASSERT(gameObject);
		ScriptContainer* container = &scriptContainerList[gameObject->scriptIndex];
		for(Script& script : container->scripts)
		{
			//context->PushState();
			asIScriptContext* context = prepareContext(script.collisionFunc);
			context->SetObject(script.scriptObj);
			context->SetArgObject(0, (void *)collisionData);
			execute(context);
			returnContext(context);
			//context->PopState();
		}
	}

	bool removeScript(GameObject* gameobject, const std::string& scriptName)
	{
		bool success = true;
		PA_ASSERT(gameobject);
		ScriptContainer* container = &scriptContainerList[gameobject->scriptIndex];
		int scriptLocation = findScriptLocation(container, scriptName);
		if(scriptLocation > -1)
		{
			Script& script = container->scripts[scriptLocation];
			script.scriptObj->Release();
			container->scripts.erase(container->scripts.begin() + scriptLocation);
			Log::message("Script '" + scriptName + "' removed from " + gameobject->name);
		}
		else
		{
			Log::error("ScriptingEngine::removeScript",
					   "Script '" + scriptName + "' not found on " + gameobject->name);
			success = false;
		}			  
		return success;
	}

	void unRegisterGameObject(GameObject* gameObject)
	{
		PA_ASSERT(gameObject);
		if(gameObject->scriptIndex >= 0)
		{
			int index = gameObject->scriptIndex;
			ScriptContainer& container = scriptContainerList[index];
			for(Script& script : container.scripts)
			{
				script.scriptObj->Release();
			}
			container.scripts.clear();
			scriptContainerEmptyIndices.push_back(index);
			int activeIndex = -1;
			for(int i = 0; i < (int)activeScriptContainers.size(); i++)
			{
				if(activeScriptContainers[i] == index)
				{
					activeIndex = i;
					break;
				}
			}
			if(activeIndex != -1)
			{
				activeScriptContainers.erase(activeScriptContainers.begin() + activeIndex);
				Log::message(gameObject->name + " unregistered from script engine");
			}
			else
			{
				Log::error("ScriptEngine::unRegisterGameobject", "Could not gameObject remove from active list");
			}
		}
		else
		{
			Log::error("ScriptEngine::unregisterGameObject", gameObject->name + " not registered");
		}
	}

	void reloadScript(const std::string& scriptName)
	{
		bool alreadyQueued = false;
		// Check if script is already queued
		for(const std::string& queuedScript : scriptReloadQueue)
		{
			if(scriptName == queuedScript)
			{
				alreadyQueued = true;
				break;
			}
		}
		if(!alreadyQueued) scriptReloadQueue.push_back(scriptName);
	}
	
	bool reloadQueuedScript(const std::string& scriptName)
	{
		// - Create temp module, compile the script, test if it has Interface implemented
		// - If it passes all checks, discard the temp module and the existing module and create new one
		// - For each script container, check if it has the script attached, remove it then add it again
		bool success = true;
		std::string moduleName = std::string(scriptName + "TEMP");
		bool moduleCreated = createModule(scriptName.c_str(), true);
		asIScriptModule* module = NULL;
		asIObjectType*   type   = NULL;
		if(moduleCreated)
		{
			// Check if it has IScriptable interface implemented
			module = engine->GetModule(moduleName.c_str(), asGM_ONLY_IF_EXISTS);
			int objectCount = module->GetObjectTypeCount();
			for(int i = 0; i < objectCount; i++)
			{
				bool found = false;
				type = module->GetObjectTypeByIndex(i);
				int interfaceCount = type->GetInterfaceCount();
				for(int j = 0; j < interfaceCount; j++)
				{
					if(strcmp(type->GetInterface(i)->GetName(), "IScriptable") == 0)
					{
						found = true;
						break;
					}
				}
				if(found) break;
			}
			if(!type)
			{
				Log::error("ScriptEngine::reloadScript", "Could not reload script '" + scriptName + "'");
				success = false;
			}
			else
			{
				std::string typeName = std::string(type->GetName());
				std::string initFuncDecl = typeName + "@ " + typeName + "(int32)";
				std::string updateFuncDecl = "void update(float)";
				std::string onCollFuncDecl = "void onCollision(const CollisionData@)";
				asIScriptFunction* initFunc   = type->GetFactoryByDecl(initFuncDecl.c_str());
				asIScriptFunction* updateFunc = type->GetMethodByDecl(updateFuncDecl.c_str());
				asIScriptFunction* onCollFunc = type->GetMethodByDecl(onCollFuncDecl.c_str());
				// Create new objects of script type and replace previous ones
				if(initFunc && updateFunc && onCollFunc)
				{
					// If any script was reloaded, match found will be true so that previous module is
					// discarded and new temp module is renamed otherwise temp module will be discarded
					bool matchFound = false;
					for(ScriptContainer& container : scriptContainerList)
					{
						GameObject* gameObject = SceneManager::find(container.gameObjectNode);
						for(Script& script : container.scripts)
						{
							if(script.module == scriptName)
							{
								matchFound = true;
								//context->PushState();
								asIScriptContext* context = prepareContext(initFunc);
								context->SetArgDWord(0, container.gameObjectNode);
								int rc = execute(context);
								if(rc == asEXECUTION_FINISHED)
								{
									// Get the newly created scriptobject and increase it's reference count
									script.scriptObj = *((asIScriptObject**)context->GetAddressOfReturnValue());
									script.scriptObj->AddRef();
									script.updateFunc = updateFunc;
									script.collisionFunc = onCollFunc;
									Log::message("Script '" + scriptName + "' reloaded for " + gameObject->name);
								}
								else if(rc == asEXECUTION_EXCEPTION)
								{
									success = false;
								}
								returnContext(context);
								//context->PushState();
							}
						}
					}
					// Discard previous module and rename temporary one
					if(matchFound)
					{
						asIScriptModule* prevModule = engine->GetModule(scriptName.c_str(), asGM_ONLY_IF_EXISTS);
						prevModule->Discard();
						module->SetName(scriptName.c_str());
					}
					else
					{
						module->Discard();
						success = false;
					}
				}
				else
				{
					Log::error("ScriotEngine::reloadScript",
							   "Required functions not implemented in '" + scriptName + "'");
					success = false;
				}
			}
		}
		else
		{
			success = false;
		}
		if(!success) Log::error("ScriptEngine::reloadScript", "Reloading '" + scriptName + "' failed");
		return success;
	}

	void executeString(const char* code)
	{
		ExecuteString(engine, code);
	}

	void saveConfigToFile()
	{
		WriteConfigToFile(engine, "angelConfig");
	}

	int getAttachedScriptsCount(GameObject* gameobject)
	{
		PA_ASSERT(gameobject);
		ScriptContainer* container = &scriptContainerList[gameobject->scriptIndex];
		return container->scripts.size();
	}
	
	const std::string getAttachedScriptName(GameObject* gameobject, int scriptLocation)
	{
		PA_ASSERT(gameobject);
		std::string scriptName = "";
		ScriptContainer* container = &scriptContainerList[gameobject->scriptIndex];
		if(scriptLocation >= 0 && scriptLocation < (int)container->scripts.size())
		{
			Script* script = &container->scripts[scriptLocation];
			scriptName = script->module;
		}
		return scriptName;
	}
}
