#include "component.h"


std::string Component::getName() const
{
    return mName;
}

void Component::setName(std::string value)
{
    mName = value;
}

Component::~Component()
{

}

Component::Component()
{
    mName = "DEFAULT_COMPONENT_NAME";
    mType = ComponentType::NO_COMPONENT;
}

ComponentType Component::getType() const
{
    return mType;
}

void Component::setType(ComponentType value)
{
    mType = value;
}
