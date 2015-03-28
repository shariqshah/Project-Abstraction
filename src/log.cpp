#include "log.h"
#include "scriptengine.h"
#include "passert.h"
#include <stdio.h>

namespace Log
{
	namespace
	{
		bool isEnabled = true;
	}
	
    void message(const std::string& message)
    {
		if(isEnabled) printf("MESSAGE : %s\n", message.c_str());
		fflush(stdout);
    }

    void error(const std::string& context, const std::string& error)
    {
		if(isEnabled) fprintf(stderr, "ERROR %s : %s\n", context.c_str(), error.c_str());
		fflush(stdout);
    }

	void warning(const std::string& warningMessage)
	{
		if(isEnabled) printf("WARNING : %s\n", warningMessage.c_str());
		fflush(stdout);
	}

	void setEnabled(bool enabled)
	{
		isEnabled = enabled;
		isEnabled ? message("Logging enabled") : message("logging disabled");
	}

	void generateBindings()
	{
		asIScriptEngine* engine = ScriptEngine::getEngine();
		engine->SetDefaultNamespace("Log");
		int rc = -1;
		rc = engine->RegisterGlobalFunction("void message(const string &in)",
											asFUNCTION(message),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("void warning(const string &in)",
											asFUNCTION(warning),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("void error(const string &in, const string &in)",
											asFUNCTION(error),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("void setEnabled(const bool)",
											asFUNCTION(setEnabled),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->SetDefaultNamespace("");
	}
}
