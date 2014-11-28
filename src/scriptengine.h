#ifndef _scriptengine_H
#define _scriptengine_H

#include <sqrat.h>
#include <sqrat/sqratVM.h>
#include <iostream>

struct GameObject;

typedef HSQUIRRELVM VM;
typedef GameObject* GOPtr;

namespace ScriptEngine
{
	VM getVM();
	
	void initialize();
	void cleanup();
	void runScript(const std::string& scriptName);
	int  createScript(const std::string& name);
	void executeFunction(const std::string& name);
	void executeFunction(const std::string& name, GOPtr gameObject);
	void executeFunction(const std::string& functionName, const float argument);
	void executeFunction(const std::string& name, GOPtr gameObject, const std::string& script);
}

#endif
