#include "gameobject.h"
#include "scriptengine.h"

namespace GO
{
    bool hasComponent(GameObject* gameObject, Component type)
	{
		if(gameObject->compIndices[(int)type] == -1)
			return false;
		else
			return true;
	}

	void attachScript(GameObject* gameObject, const std::string& name)
	{
		int scriptHandle = ScriptEngine::createScript(name);
		
		if(scriptHandle >= 0)
		{
			gameObject->scripts.push_back(scriptHandle);
			Log::message("Script " + name + " added to " + gameObject->name);
		}
		else
		{
			Log::error("GameObject", "Could not attach script " + name +
					   "to " + gameObject->name);
		}
	}

	void generateBindings()
	{
		Sqrat::RootTable().Bind("GameObject", Sqrat::Class<GameObject>()
								.Var("node",   &GameObject::node)
								.Var("name",   &GameObject::name)
								.Var("tag",    &GameObject::tag)
								.Var("remove", &GameObject::remove));

		Sqrat::RootTable().Bind("GO", Sqrat::Table (ScriptEngine::getVM())
								.Func("hasComponent", &hasComponent));
	}

	void updateScripts(GameObject* gameObject, float deltaTime)
	{
		// for(int script : gameObject->scripts)
		// 	ScriptEngine::exectuteScriptFunction(script, "update", deltaTime);
	}
}
