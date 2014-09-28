#ifndef COMPONENT_H
#define COMPONENT_H

#include <iostream>

#include "componentTypes.h"

class Component
{
protected:
    std::string mName;
    ComponentType mType;
	bool mValid;
public:
    virtual ~Component();
    Component();
    std::string getName() const;
    ComponentType getType() const;
	bool isValid();
};

#endif // COMPONENT_H
