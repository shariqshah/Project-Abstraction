#include "scenemanager.h"

namespace SceneManager
{
	namespace
	{
		static GOMap sSceneObjects;
		static std::vector<Node> sRemovables;
	}
	
	bool add(GOPtr newGameObject)
	{
		Node node = newGameObject->getNode();
		const std::string name = newGameObject->getName();
		
		std::pair<GOMap::iterator, bool> returnValue;
		returnValue = sSceneObjects.insert(std::make_pair(node, newGameObject));

		if(returnValue.second)
		{
			Log::message(name + " added to scene");
			return true;
		}
		else
		{
			Log::error(Log::ErrorLevel::MEDIUM,
					   name + " could not be added to scene");
			return false;
		}
	}

	bool remove(Node node)
	{
		auto position = sSceneObjects.find(node);

		if(position != sSceneObjects.end())
		{
			//Recursively remove all children first, if any.
			GOArray children;
			if(getChildren(position->second.get(), &children))
			{
				std::for_each(children.begin(), children.end(), [] (GOPtr child)
  			    {
				    remove(child->getNode());
				});
			}

			sRemovables.push_back(node);
			Log::message(position->second->getName() + " removed from scene");
			return true;
		}

		Log::error(Log::ErrorLevel::LOW,
				   "GO " + std::to_string(node) +
				   " not found in scene so cannot be removed.");
		return false;
	}
	
	bool remove(const std::string& name)
	{
		for(GOMap::iterator it = sSceneObjects.begin();
			it != sSceneObjects.end();
			++it)
		{
			if(name == it->second->getName())
			{
				//Recursively remove all children first, if any.
				GOArray children;
				if(getChildren(it->second.get(), &children))
				{
					std::for_each(children.begin(), children.end(), [] (GOPtr child)
					{
					    remove(child->getNode());
					});
				}
				sRemovables.push_back(it->first);
				Log::message(name + " removed from scene");
				return true;
			}
		}

		Log::error(Log::ErrorLevel::LOW,
				   name + " not found in scene so cannot be removed.");
		return false;
	}
	
	GOPtr find(const std::string& name)
	{
		for(GOMap::iterator it = sSceneObjects.begin();
			it != sSceneObjects.end();
			++it)
		{
			if(name == it->second->getName())
				return it->second;
		}

		Log::warning(name + " not found in scene");
		return nullptr;
	}

	GOPtr find(Node node)
	{
	    auto it = sSceneObjects.find(node);

		if(it != sSceneObjects.end())
			return it->second;

		Log::warning("GO " + std::to_string(node) + " not found in scene");
		return nullptr;
	}

	void update()
	{
		//Remove Marked GOs
		for(Node node : sRemovables)
		{
			int removed = sSceneObjects.erase(node);
			if(removed == 0)
				Log::warning("GO marked for removal could not be removed!");
		}

		sRemovables.clear();
	}

	void cleanup()
	{
		std::cout<<"Size before : "<<sSceneObjects.size()<<std::endl;
		sSceneObjects.clear();
		std::cout<<"Size after : "<<sSceneObjects.size()<<std::endl;

		if(!sRemovables.empty())
			sRemovables.clear();
	}

	GOPtr createGameObject(const std::string& name)
	{
		GOPtr newObj = std::make_shared<GameObject>(name);
		newObj->addComponent<Transform>(newObj->getNode());
		add(newObj);
		
		return newObj;
	}

	GOMap* getSceneObjects()
	{
		return &sSceneObjects;
	}

	GOPtr getParent(GameObject* gameObject)
	{
		if(gameObject)
		{
			Node parent = Renderer::getParent(gameObject->getNode());
			
			if(parent != 0)
				return find(parent);
		}
		
		return nullptr;
	}

	GOPtr getChild(GameObject* gameObject, const std::string& name)
	{
		GOArray children;

		if(getChildren(gameObject, &children, name))
			return children[0];
		
		return nullptr;
	}

	bool getChildren(GameObject* gameObject,
					 GOArray* children,
					 const std::string& name)
	{
		NodeArray childNodes;

		if(Renderer::getNodeChildren(gameObject->getNode(), name, &childNodes))
		{
			for(Node child : childNodes)
				children->push_back(find(child));

			return true;
		}
		
		return false;
	}

	void syncTransform(GameObject* gameObject)
	{
		glm::vec3 position, rotation, scale;
		Renderer::getNodeTransform(gameObject->getNode(),
								   &position,
								   &rotation,
								   &scale);
			
		auto transform = gameObject->getComponent<Transform>();
		transform->setPosition(position, false);
		transform->setRotation(glm::quat(rotation), false);
		transform->setScale(scale, false);
	}

	bool setParent(GameObject* child, GameObject* parent)
	{
		if(Renderer::setParent(child->getNode(), parent->getNode()))
		{
			syncTransform(child);
			return true;
		}

		return false;
	}

	bool setParentAsRoot(GameObject* gameObject)
	{
		if(Renderer::setParent(gameObject->getNode(), Renderer::Root))
		{
			syncTransform(gameObject);
			return true;
		}

		return false;
	}
}
