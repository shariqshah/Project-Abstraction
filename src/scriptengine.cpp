#include "scriptengine.h"
#include <stdarg.h>
#include <vector>
#include "log.h"
#include "gameobject.h"
#include "utilities.h"
#include "physics.h"
#include "../include/angelscript/add_on/scriptstdstring/scriptstdstring.h"

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
}
