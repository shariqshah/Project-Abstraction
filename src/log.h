#ifndef _log_H
#define _log_H

#include <iostream>

namespace Log
{
    enum class ErrorLevel { LOW = 0, MEDIUM, CRITICAL };

    void message(const std::string message);
    void error(ErrorLevel level, const std::string error);
	void warning(const std::string warningMessage);
}


#endif
