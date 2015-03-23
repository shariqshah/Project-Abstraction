#include "log.h"
#include "scriptengine.h"

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
    }

    void error(const std::string& context, const std::string& error)
    {
		if(isEnabled) fprintf(stderr, "ERROR %s : %s\n", context.c_str(), error.c_str());
    }

	void warning(const std::string& warningMessage)
	{
		if(isEnabled) printf("WARNING : %s\n", warningMessage.c_str());
	}

	void setEnabled(bool enabled)
	{
		isEnabled = enabled;
		isEnabled ? message("Logging enabled") : message("logging disabled");
	}

	void generateBindings()
	{
		Sqrat::RootTable().Bind("Log", Sqrat::Table(ScriptEngine::getVM())
								.Func("message", &message)
								.Func("error", &error)
								.Func("warning", &warning)
								.Func("setEnabled", &setEnabled));

		asIScriptEngine* engine = ScriptEngine::getEngine();
		engine->SetDefaultNamespace("Log");
		int rc = -1;
		rc = engine->RegisterGlobalFunction("void message(const string)",
											asFUNCTION(message),
											asCALL_CDECL);
		assert(rc >= 0);
		rc = engine->RegisterGlobalFunction("void warning(const string)",
											asFUNCTION(warning),
											asCALL_CDECL);
		assert(rc >= 0);
		rc = engine->RegisterGlobalFunction("void error(const string, const string)",
											asFUNCTION(error),
											asCALL_CDECL);
		assert(rc >= 0);
		rc = engine->RegisterGlobalFunction("void setEnabled(const bool)",
											asFUNCTION(setEnabled),
											asCALL_CDECL);
		assert(rc >= 0);
		engine->SetDefaultNamespace("");
	}
}
