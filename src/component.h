#ifndef COMPONENT_H
#define COMPONENT_H

#include <iostream>

#include "componentTypes.h"

class Component
{
protected:
    std::string mName;
    ComponentType mType;
public:
    virtual ~Component();
    Component();
    std::string getName() const;
    ComponentType getType() const;
    void setName(std::string value);
    void setType(ComponentType value);
};

#endif // COMPONENT_H
