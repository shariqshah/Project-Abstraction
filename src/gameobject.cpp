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
}
