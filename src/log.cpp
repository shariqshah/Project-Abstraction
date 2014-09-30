#include "log.h"

namespace Log
{
    void message(const std::string message)
    {
        std::cout<<"MESSAGE : " + message<<std::endl;
    }

    void error(Log::ErrorLevel level, const std::string error)
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

	void warning(const std::string warningMessage)
	{
		std::cout<<"WARNING : " + warningMessage<<std::endl;
	}
}
