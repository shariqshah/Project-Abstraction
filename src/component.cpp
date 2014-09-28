#include "component.h"


std::string Component::getName() const
{
    return mName;
}

Component::~Component()
{

}

Component::Component()
{
    mName = "DEFAULT_COMPONENT_NAME";
    mType = ComponentType::NO_COMPONENT;
	mValid = true;
}

ComponentType Component::getType() const
{
    return mType;
}

bool Component::isValid()
{
	return mValid;
}
