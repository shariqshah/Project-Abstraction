#include "scriptengine.h"
#include <stdarg.h>
#include <vector>
#include "log.h"
#include "gameobject.h"
#include "utilities.h"
#include "physics.h"
#include "datatypes.h"
#include "../include/angelscript/add_on/scriptstdstring/scriptstdstring.h"
#include "../include/angelscript/add_on/scriptbuilder/scriptbuilder.h"

struct Script
{
	bool                  enabled;
	std::string           module;
	asIScriptFunction*    updateFunc = NULL;
	asIScriptFunction*    initFunc   = NULL;
	asIObjectType*        objType    = NULL;
	asIScriptObject*  scriptObj  = NULL;
};

struct ScriptContainer
{
	std::vector<Script> scripts;
	Node gameObjectNode;
};

namespace ScriptEngine
{
	namespace
	{
		VM vmInstance;
		Sqrat::SqratVM *vm;
		std::vector<Sqrat::Script> scriptList;
		std::vector<int> scriptFreeIndices;

		asIScriptEngine*  engine = NULL;
		asIScriptContext* context = NULL;
		std::vector<ScriptContainer> scriptContainerList;
		std::vector<int> scriptContainerEmptyIndices;
		std::vector<int> activeScriptContainers;
		std::string scriptDir = "../content/scripts/"; // TODO: Find a better way to do this

		int execute()
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
	}

	void printfunc(HSQUIRRELVM v,const SQChar *s,...)
	{
		va_list vl;
		va_start(vl, s);
		vfprintf(stdout, s, vl);
		va_end(vl);
	}

	void errorfunc(HSQUIRRELVM v,const SQChar *s,...)
	{
		va_list vl;
		va_start(vl, s);
		vfprintf(stderr, s, vl);
		va_end(vl);
	}

	void MessageCallback(const asSMessageInfo *msg, void *param)
	{
		const char *type = "ERR ";
		if( msg->type == asMSGTYPE_WARNING ) 
			type = "WARN";
		else if( msg->type == asMSGTYPE_INFORMATION ) 
			type = "INFO";

		printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
	}

	
	bool initialize()
	{
		bool success = true;
		vm = new Sqrat::SqratVM(1024);
		vm->SetPrintFunc(printfunc, errorfunc);
		vmInstance = vm->GetVM();
		Sqrat::DefaultVM::Set(vm->GetVM());

		//////////////////////////////////////////////////////////////////////////////////////////
		
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
			context = engine->CreateContext();
			RegisterStdString(engine); // Register string type

			engine->RegisterInterface("IScriptable");
			engine->RegisterInterfaceMethod("IScriptable", "void update(float)");
			engine->RegisterInterfaceMethod("IScriptable", "void initialize(int32)");
		}
		return success;
	}

	void cleanup()
	{
		for(Sqrat::Script script : scriptList)
			script.Release();

		scriptList.clear();
		delete vm;

		////////////////////////////////////////////////////////////////////////////////////////////

		if(engine)
		{
			context->Release();
			engine->ShutDownAndRelease();
		}
	}

	VM getVM()
	{
		return vmInstance;
	}

	void runScript(const std::string& scriptName)
	{
		Sqrat::Script script(vmInstance);
		std::string error;
		
		if(script.CompileFile(scriptName, error))
		{
			Log::message("Script : " + scriptName + " compiled");
			if(!script.Run(error))
				Log::error("Script Runtime", error);
			else
				script.Release();
		}
		else
			Log::error("Script Compile", error);
	}

	int createScript(const std::string& name)
	{
		Sqrat::Script newScript;
		std::string error;
		
		if(newScript.CompileFile(name, error))
		{
			if(newScript.Run(error))
			{
				int index = -1;
				if(!scriptFreeIndices.empty())
				{
					index = scriptFreeIndices.back();
					scriptList[index] = newScript;
				}
				else
				{
					scriptList.push_back(newScript);
					index = scriptList.size() - 1;
				}

				return index;
			}
			else
			{
				Log::error("Script Runtime", error);
				return -1;
			}
			
		}
		else
		{
			Log::error("Compiling " + name, error);
			return -1;
		}	
	}

	bool addScript(const std::string& name)
	{
		bool success = true;
		char* scriptText = Utils::loadFileIntoCString(name.c_str());
		if(scriptText && engine)
		{
			asIScriptModule *module = engine->GetModule(0, asGM_ALWAYS_CREATE);
			int rc = module->AddScriptSection(name.c_str(), scriptText, strlen(scriptText));
			if(rc < 0)
			{
				success = false;
				Log::error("ScriptEngine::addScript", "Adding module failed");
			}
			else
			{
				rc = module->Build();
				if(rc < 0)
				{
					success = false;
					Log::error("ScriptEngine::addScript", "Building script " + name + " failed");
				}
				else
				{
					Log::message("Script " + name + " added successfully");
				}
			}
			free(scriptText);
		}
		else
		{
			Log::error("ScriptEngine::addScript", name + " not found");
			success = false;
		}
		return success;
	}

	void executeFunction(const char* declaration)
	{
		asIScriptFunction* function = engine->GetModule(0)->GetFunctionByDecl(declaration);
		if(function == 0)
		{
			Log::error("ScriptEngine::executeFunction",
					   "Function : " + std::string(declaration) + " not found");
			assert(function != 0);
			return;
		}
		context->Prepare(function);
		context->Execute();
	}

	asIScriptEngine* getEngine()
	{
		return engine;
	}

	void registerGameObject(GameObject* gameObject)
	{
		assert(gameObject);
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

	bool createModule(const std::string& scriptName)
	{
		bool success = true;
		CScriptBuilder builder;
		int rc = builder.StartNewModule(engine, scriptName.c_str());
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
					Log::error("ScriptEngine::createModule", "Error building module " + scriptName);
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
		assert(gameObject);
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
					   "No class implmenting ISciptable interface found in " + scriptName);
			Log::error("ScriptEngine::addScript", scriptName + " not added to " + gameObject->name);
			container->scripts.pop_back();
			return;
		}

		// Find the update and init functions in the interface
		std::string typeName = std::string(type->GetName());
		std::string initFuncDecl = typeName + "@ " + typeName + "(int32)";
		std::string updateFuncDecl = "void update(float)";
		newScript.initFunc = type->GetFactoryByDecl(initFuncDecl.c_str());
		newScript.updateFunc = type->GetMethodByDecl(updateFuncDecl.c_str());

		// Create the script object
		context->Prepare(newScript.initFunc);
		context->SetArgDWord(0, gameObject->node);
		int rc = execute();
		if(rc == asEXECUTION_FINISHED)
		{
			// Get the newly created scriptobject and increase it's reference count
			newScript.scriptObj = *((asIScriptObject**)context->GetAddressOfReturnValue());
			newScript.scriptObj->AddRef();
			Log::message(scriptName + " added to " + gameObject->name);
		}
		else if(rc == asEXECUTION_EXCEPTION)
		{
			Log::error("ScriptEngine::addScript", scriptName + " not added to " + gameObject->name);
			container->scripts.pop_back();
		}
	}

	void updateAllScripts(float deltaTime)
	{
		for(int activeContainer : activeScriptContainers)
		{
			ScriptContainer* container = &scriptContainerList[activeContainer];
			for(Script& script : container->scripts)
			{
				if(script.enabled)
				{
					context->Prepare(script.updateFunc);
					context->SetObject(script.scriptObj);
					context->SetArgFloat(0, deltaTime);
					execute();
				}
			}
		}
	}

	void unRegisterGameObject(GameObject* gameObject)
	{
		assert(gameObject);
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
}
