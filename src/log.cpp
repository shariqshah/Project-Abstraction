#include "log.h"

namespace Log
{
    void message(std::string message)
    {
        std::cout<<"MESSAGE : " + message<<std::endl;
    }

    void error(Log::ErrorLevel level, std::string error)
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
