#include "scriptengine.h"
#include <stdarg.h>
#include <vector>
#include "log.h"
#include "gameobject.h"

namespace ScriptEngine
{
	namespace
	{
		static VM vmInstance;
		static Sqrat::SqratVM *vm;
		static std::vector<Sqrat::Script> scriptList;
		static std::vector<int> scriptFreeIndices;
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

	void executeFunction(const std::string& name, const float argument)
	{
		Sqrat::Function function = Sqrat::RootTable().GetFunction(name.c_str());

		if(function.IsNull())
			Log::error("executefunction", "Could not find function called : " + name);
		else
			function.Execute(argument);
	}

	void executeFunction(const std::string& name)
	{
		Sqrat::Function function = Sqrat::RootTable().GetFunction(name.c_str());

		if(function.IsNull())
			Log::error("executefunction", "Could not find function called : " + name);
		else
			function.Execute();
	}

	void executeFunction(const std::string& name, GOPtr gameObject, const std::string& script)
	{
		Sqrat::Function function = Sqrat::RootTable().GetFunction(name.c_str());

		if(function.IsNull())
			Log::error("executefunction", "Could not find function called : " + name);
		else
			function.Execute(gameObject, script);
	}

	void executeFunction(const std::string& name, GOPtr gameObject)
	{
		Sqrat::Function function = Sqrat::RootTable().GetFunction(name.c_str());

		if(function.IsNull())
			Log::error("executefunction", "Could not find function called : " + name);
		else
			function.Execute(gameObject);
	}

	void executeFunction(const std::string& name,
						 const std::string& goName,
						 const std::string& scriptName)
	{
		Sqrat::Function function = Sqrat::RootTable().GetFunction(name.c_str());

		if(function.IsNull())
			Log::error("executefunction", "Could not find function called : " + name);
		else
			function.Execute(goName, scriptName);
	}
}
