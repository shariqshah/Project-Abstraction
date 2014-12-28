#include "log.h"
#include "scriptengine.h"

namespace Log
{
	namespace
	{
		bool isEnabled = true;
	}
	
    void message(const std::string& message)
    {
		if(isEnabled)
			std::cout<<"MESSAGE : " + message<<std::endl;
    }

    void error(const std::string& context, const std::string& error)
    {
		if(isEnabled)
			std::cerr<<"ERROR " + context + " : " + error<<std::endl;
    }

	void warning(const std::string& warningMessage)
	{
		if(isEnabled)
			std::cout<<"WARNING : " + warningMessage<<std::endl;
	}

	void setEnabled(bool enabled)
	{
		isEnabled = enabled;
		
		if(isEnabled)
			message("Logging enabled");
		else
			message("Logging disabled");
	}

	void generateBindings()
	{
		Sqrat::RootTable().Bind("Log", Sqrat::Table(ScriptEngine::getVM())
								.Func("message", &message)
								.Func("error", &error)
								.Func("warning", &warning)
								.Func("setEnabled", &setEnabled));
	}
}
