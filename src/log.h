#ifndef _log_H
#define _log_H

#include <iostream>

namespace Log
{
    void message(const std::string& message);
    void error(const std::string& context, const std::string& error);
	void warning(const std::string& warningMessage);
	void setEnabled(bool enabled);
}


#endif
