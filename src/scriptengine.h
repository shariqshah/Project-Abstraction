#ifndef _scriptengine_H
#define _scriptengine_H

#include <sqrat.h>
#include <sqrat/sqratVM.h>
#include <iostream>

typedef HSQUIRRELVM VM;

namespace ScriptEngine
{
	VM getVM();
	
	void initialize();
	void cleanup();
	void runScript(const std::string& scriptName);
}

#endif
