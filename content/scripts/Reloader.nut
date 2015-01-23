dofile("../content/scripts/BaseBehaviour.nut", true);

class Reloader extends BaseBehaviourScript </ type = "Reloader" />
{
	speed = null;
	
	constructor(attachedObj)
	{
		base.constructor(attachedObj);
		speed = 50.0;
	}

	function update(deltaTime)
	{
		if(Input.isKeyReleased(Key.V))
		{
			local player = SceneManager.findByName("TestOBJ")
			::reloadScript(player, "Test")
		}

		if(Input.isKeyReleased(Key.R))
			::reloadScript(getGameObject(), "Reloader")
	}
}

function init(gameObject)
{
	if(!gameObject)
	{
		Log.error("Reloader Init", "Invalid gameObject!");
		return null;
	}
	else
	{
		return Reloader(gameObject);
	}
}

return init;
