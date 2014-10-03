#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <memory>
#include <unordered_map>

#include "component.h"
#include "renderer.h"

typedef std::unordered_map<std::string, std::shared_ptr<Component>> ComponentMap;
typedef std::shared_ptr<Component> ComponentPtr;

class GameObject
{
    //TODO: Add measures for ensuring multiple instances of same component type
    //cannot be added
	//TODO: Add measures for getting child object if any
protected:
	Node mNode;
    std::string mName;
    std::string mTag;
    ComponentMap mComponentMap;
	bool mRemove;
    long mComponentMask;

    ComponentPtr getComponent(const std::string &componentType);
public:
    GameObject(const std::string name = "Defaultgameobjectname",
			   const std::string tag  = "DefaultTag");
	~GameObject();

	template<typename T, typename... Args>
    std::shared_ptr<T> addComponent(Args&&... args)
    {
		auto component = std::make_shared<T>(args...);
        return std::dynamic_pointer_cast<T>(addComponent(component));
    }
	
    ComponentPtr addComponent(ComponentPtr component);

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
