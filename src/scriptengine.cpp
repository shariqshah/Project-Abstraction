#include "scriptengine.h"
#include <stdarg.h>
#include <vector>
#include "log.h"
#include "gameobject.h"
#include "physics.h"

namespace ScriptEngine
{
	namespace
	{
		VM vmInstance;
		Sqrat::SqratVM *vm;
		std::vector<Sqrat::Script> scriptList;
		std::vector<int> scriptFreeIndices;
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
	
	void initialize()
	{
		vm = new Sqrat::SqratVM(1024);
		vm->SetPrintFunc(printfunc, errorfunc);
		vmInstance = vm->GetVM();
		Sqrat::DefaultVM::Set(vm->GetVM());
	}

	void cleanup()
	{
		for(Sqrat::Script script : scriptList)
			script.Release();

		scriptList.clear();
		delete vm;
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
}
