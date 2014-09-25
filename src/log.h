#ifndef _log_H
#define _log_H

#include <iostream>

namespace Log
{
    enum class ErrorLevel { LOW = 0, MEDIUM, CRITICAL };

    void message(std::string message);
    void error(ErrorLevel level, std::string error);
}


#endif
