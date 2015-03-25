#ifndef _scriptengine_H
#define _scriptengine_H

#include <iostream>
#include <sqrat.h>
#include <sqrat/sqratVM.h>
#include "../include/angelscript/angelscript.h"

#include "log.h"


struct GameObject;
struct CollisionData;

typedef HSQUIRRELVM VM;

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
	void updateAllScripts(float deltaTime);
	void addScript(GameObject* gameObject, const std::string& scriptName);
    void unRegisterGameObject(GameObject* gameObject);
    void registerGameObject(GameObject* gameObject);	
	asIScriptEngine* getEngine();
}

#endif
