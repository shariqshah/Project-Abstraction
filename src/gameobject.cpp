#include "gameobject.h"

std::string GameObject::getName() const
{
    return mName;
}

void GameObject::setName(const std::string name)
{
    mName = name;
	h3dSetNodeParamStr(mNode, H3DNodeParams::NameStr, mName.c_str());
}

std::shared_ptr<Component> GameObject::getComponent(const std::string &componentType)
{
    ComponentMap::iterator it;
    it = mComponentMap.find(componentType);

    if(it == mComponentMap.end())
    {
        Log::error(Log::ErrorLevel::MEDIUM,
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

void GameObject::setTag(std::string tag)
{
    mTag = tag;
}

bool GameObject::compareTag(std::string tagToCompare)
{
    if(mTag == tagToCompare)
        return true;
    else
        return false;
}

GameObject::GameObject()
{
    mName = "DefaultGameObjectName";
    mTag = "DefaultTag";
    mComponentMask = (long)ComponentType::NO_COMPONENT;
	mNode = h3dAddGroupNode(H3DRootNode, mName.c_str());
}

void GameObject::AddComponent(std::shared_ptr<Component> component)
{
    ComponentType type = component.get()->getType();
    std::string name = component.get()->getName();
    std::pair<ComponentMap::iterator, bool> returnValue;
    returnValue = mComponentMap.insert(std::make_pair(name, component));

    if(returnValue.second)
    {
        mComponentMask |= (long)type;
        Log::message(name + " component added to " + mName);
    }
    else
    {
        Log::error(Log::ErrorLevel::MEDIUM,
                   name + " component could not be added to " + mName);
    }
}

void GameObject::removeComponent(std::string componentName)
{
    auto position = mComponentMap.find(componentName);

    if(position != mComponentMap.end())
    {
        ComponentType type = position->second.get()->getType();
        mComponentMask &= ~(long)type;
        mComponentMap.erase(position);
        Log::message(componentName + " removed from " + mName);

    }
    else
    {
        Log::error(Log::ErrorLevel::MEDIUM,
                   "Component " + componentName + " not removed from " + mName);
    }
//    if(componentsRemoved < 1)
//    {
//        Log::error(Log::ErrorLevel::MEDIUM,
//                   "Component " + componentName + " not removed from " + mName);
//    }
//    else
//    {
//        mComponentMask &= ~(long)Co
//        Log::message(componentsRemoved + " components removed from " + mName);
//    }

}

Node GameObject::getNode()
{
	return mNode;
}
