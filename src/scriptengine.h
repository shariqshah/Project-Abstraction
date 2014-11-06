#ifndef _scriptengine_H_
#define _scriptengine_H_

#include <iostream>
#include <stdarg.h>
#include <sqrat.h>
#include <sqrat/sqratVM.h>

#include "log.h"


typedef HSQUIRRELVM VM;

namespace ScriptEngine
{
	VM getVM();
	
	void initialize();
	void cleanup();
	void runScript(const std::string& scriptName);
}

#endif
