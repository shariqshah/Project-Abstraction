#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <memory>
#include <unordered_map>

#include "../include/horde3d/Horde3D.h"

#include "component.h"
#include "log.h"

typedef H3DNode Node;

class GameObject
{
    //TODO: Add measures for ensuring multiple instances of same component type
    //cannot be added
protected:
    typedef std::unordered_map<std::string, std::shared_ptr<Component>> ComponentMap;

	Node mNode;
    std::string mName;
    std::string mTag;
    ComponentMap mComponentMap;
    long mComponentMask;

    std::shared_ptr<Component> getComponent(const std::string &componentType);
public:
    GameObject();
    void AddComponent(std::shared_ptr<Component>component);

    void removeComponent(std::string componentName);
    void setName(const std::string value);
    void setComponentMask(long value);
    bool hasComponents(long componentMask);
    void setTag(std::string value);
    bool compareTag(std::string tagToCompare);

    std::string getName() const;
    long getComponentMask() const;
	std::string getTag() const;
	Node getNode();

	template<typename T>
    std::shared_ptr<T> getComponent(std::string componentType)
    {
        return std::dynamic_pointer_cast<T>(getComponent(componentType));
    }
};

#endif // GAMEOBJECT_H
