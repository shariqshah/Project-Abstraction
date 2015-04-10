#ifndef _scriptengine_H
#define _scriptengine_H

#include <iostream>
#include "../include/angelscript/angelscript.h"

#include "log.h"

struct GameObject;
struct CollisionData;

namespace ScriptEngine
{	
	bool initialize();
	void cleanup();
	void executeString(const char* filename);
	void updateAllScripts(float deltaTime);
	void addScript(GameObject* gameObject, const std::string& scriptName);
    void unRegisterGameObject(GameObject* gameObject);
	void callCollisionCallcallbacks(GameObject* gameObject, const CollisionData* collisionData);
    void registerGameObject(GameObject* gameObject);
	bool removeScript(GameObject* gameobject, const std::string& scriptName);
	void reloadScript(const std::string& scriptName);
	asIScriptEngine* getEngine();
	void registerScriptInterface();
	void saveConfigToFile();
}

#endif
