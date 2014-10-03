#include "gameobject.h"

std::string GameObject::getName() const
{
    return mName;
}

void GameObject::setName(const std::string& name)
{
    mName = name;
	Renderer::setNodeName(mNode, mName);
}

ComponentPtr GameObject::getComponent(const std::string &componentType)
{
    ComponentMap::iterator it;
    it = mComponentMap.find(componentType);

    if(it == mComponentMap.end())
    {
        Log::error(Log::ErrorLevel::LOW,
                   componentType + " not found on object " + mName);
        return nullptr;
    }

    return it->second;
}

long GameObject::getComponentMask() const
{
    return mComponentMask;
}

void GameObject::setComponentMask(long value)
{
    mComponentMask = value;
}

bool GameObject::hasComponents(long componentMask)
{
    if((mComponentMask & componentMask) == componentMask)
        return true;
    else
        return false;
}

std::string GameObject::getTag() const
{
    return mTag;
}

void GameObject::setTag(const std::string& tag)
{
    mTag = tag;
}

bool GameObject::compareTag(const std::string& tagToCompare)
{
    if(mTag == tagToCompare)
        return true;
    else
        return false;
}

GameObject::GameObject(const std::string& name, const std::string& tag)
{
    mName = name;
    mTag = tag;
    mComponentMask = (long)ComponentType::NO_COMPONENT;
	mNode = Renderer::createGroupNode(mName.c_str());
	mRemove = false;
}

GameObject::~GameObject()
{
	if(mNode != 0)
	{	
		Renderer::removeNode(mNode);
	}
}

ComponentPtr GameObject::addComponent(ComponentPtr component)
{
	ComponentType type = component->getType();
	std::string name = component->getName();
		
	if(component->isValid())
	{
		std::pair<ComponentMap::iterator, bool> returnValue;
		returnValue = mComponentMap.insert(std::make_pair(name, component));

		if(returnValue.second)
		{
			mComponentMask |= (long)type;
			Log::message(name + " component added to " + mName);
		}
		else
		{
			Log::warning(name + " component could not be added to " + mName +
					     " because it already exits");
		}

		return returnValue.first->second; //return the component
	}
	else
	{
		Log::error(Log::ErrorLevel::MEDIUM,
				   name + " component could not be added to " + mName +
				   ". Component Invalid!");
	}

	return nullptr;
}

void GameObject::removeComponent(const std::string& componentName)
{
    auto position = mComponentMap.find(componentName);

    if(position != mComponentMap.end())
    {
		auto component = position->second;
        ComponentType type = component->getType();
        mComponentMask &= ~(long)type;
        mComponentMap.erase(position);
        Log::message(componentName + " component removed from " + mName);
    }
    else
    {
        Log::error(Log::ErrorLevel::MEDIUM,
                   "Component " + componentName + " not removed from " + mName);
    }
}

Node GameObject::getNode() const
{
	return mNode;
}

void GameObject::markForRemoval()
{
	mRemove = true;
}

bool GameObject::isMarkedForRemoval()
{
	return mRemove;
}


