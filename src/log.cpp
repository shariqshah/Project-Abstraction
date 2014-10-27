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

    void error(Log::ErrorLevel level, const std::string& error)
    {
		if(isEnabled)
		{
			std::string levelString = "";

			if(level == Log::ErrorLevel::LOW)
				levelString = "LOW";
			else if(level == Log::ErrorLevel::MEDIUM)
				levelString = "MEDIUM";
			else if(level == Log::ErrorLevel::CRITICAL)
				levelString = "CRITICAL";

			std::cout<<"ERROR " + levelString + " : " + error<<std::endl;
		}
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
