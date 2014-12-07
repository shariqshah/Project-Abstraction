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

	void attachScript(GameObject* gameObject, const std::string& name)
	{
		ScriptEngine::executeFunction("attachScript", gameObject, name);
	}

	void reloadScript(GameObject* gameObject, const std::string& name)
	{
		ScriptEngine::executeFunction("reloadScript", gameObject, name);
	}

	void reloadScript(const std::string& goName, const std::string& name)
	{
		ScriptEngine::executeFunction("reloadScript", goName, name);
	}

	void reloadAllScripts(GameObject* gameObject)
	{
		ScriptEngine::executeFunction("reloadAllScripts", gameObject);
	}

	void reloadAllScripts(const std::string& goName)
	{
		ScriptEngine::executeFunction("reloadAllScripts", goName);
	}
	
	void removeScript(GameObject* gameObject, const std::string& name)
	{
		ScriptEngine::executeFunction("removeScript", gameObject, name);
	}

	void reloadScriptType(const std::string& typeName)
	{
		ScriptEngine::executeFunction("reloadScriptType", typeName);
	}
}
