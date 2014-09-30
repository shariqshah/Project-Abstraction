#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <memory>
#include <unordered_map>

#include "component.h"
#include "renderer.h"

class GameObject
{
    //TODO: Add measures for ensuring multiple instances of same component type
    //cannot be added
	//TODO: Add measures for getting child object if any
protected:
    typedef std::unordered_map<std::string, std::shared_ptr<Component>> ComponentMap;

	Node mNode;
    std::string mName;
    std::string mTag;
    ComponentMap mComponentMap;
	bool mRemove;
    long mComponentMask;

    std::shared_ptr<Component> getComponent(const std::string &componentType);
public:
    GameObject();
	~GameObject();
	
    void addComponent(std::shared_ptr<Component>component);

    void removeComponent(std::string componentName);
    void setName(const std::string value);
    void setComponentMask(long value);
	void markForRemoval();
    void setTag(std::string value);
	
    bool compareTag(std::string tagToCompare);
	bool isMarkedForRemoval();
	bool hasComponents(long componentMask);

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
