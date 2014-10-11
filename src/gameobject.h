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
	//TODO: Add measures for getting child object if any
protected:
	Node         mNode;
    std::string  mName;
    std::string  mTag;
    ComponentMap mComponentMap;
	bool         mRemove;
    long         mComponentMask;

    ComponentPtr getComponent(const std::string& componentType);
	ComponentPtr addComponent(ComponentPtr component);
public:
    GameObject(const std::string& name = "Defaultgameobjectname",
			   const std::string& tag  = "DefaultTag");
	~GameObject();

	template<typename T, typename... Args>
    std::shared_ptr<T> addComponent(Args&&... args)
    {
		auto component = std::make_shared<T>(args...);
        return std::dynamic_pointer_cast<T>(addComponent(component));
    }

    void setName(const std::string& value);
    void setComponentMask(long value);
    void setTag(const std::string& value);
	void markForRemoval();
	void removeComponent(const std::string& componentName);
	
    bool compareTag(const std::string& tagToCompare);
	bool isMarkedForRemoval();
	bool hasComponents(long componentMask);

    std::string getName() const;
	std::string getTag() const;
    long        getComponentMask() const;
	Node        getNode() const;

	template<typename T>
    std::shared_ptr<T> getComponent()
    {
        return std::dynamic_pointer_cast<T>(getComponent(T::sName));
    }
};

#endif // GAMEOBJECT_H
