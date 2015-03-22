#ifndef _scriptengine_H
#define _scriptengine_H

#include <sqrat.h>
#include <sqrat/sqratVM.h>
#include "../include/angelscript/angelscript.h"

#include "log.h"

struct GameObject;
struct CollisionData;

typedef HSQUIRRELVM VM;
typedef GameObject* GOPtr;

namespace ScriptEngine
{
	VM getVM();
	
	bool initialize();
	void cleanup();
	void runScript(const std::string& scriptName);
	int  createScript(const std::string& name);

	template<typename... Params>
	void executeFunction(const std::string& name, Params... params)
	{
		Sqrat::Function function = Sqrat::RootTable().GetFunction(name.c_str());

		if(function.IsNull())
			Log::error("executefunction", "Could not find function called : " + name);
		else
			function.Execute(params...);
	}

	bool addScript(const std::string& name);
	void executeFunction(const char* declaration);

	asIScriptEngine* getEngine();
}

#endif
