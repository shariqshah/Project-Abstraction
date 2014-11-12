#include "log.h"

namespace Log
{
	namespace
	{
		static bool isEnabled = true;
	}
	
    void message(const std::string& message)
    {
		if(isEnabled)
			std::cout<<"MESSAGE : " + message<<std::endl;
    }

    void error(const std::string& context, const std::string& error)
    {
		if(isEnabled)
			std::cout<<"ERROR " + context + " : " + error<<std::endl;
    }

	void warning(const std::string& warningMessage)
	{
		if(isEnabled)
			std::cout<<"WARNING : " + warningMessage<<std::endl;
	}

	void setEnabled(bool enabled)
	{
		isEnabled = enabled;
	}
}
