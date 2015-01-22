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
			local player = SceneManager.findByName("Player")
			::reloadScript(player, "PlayerBehaviour")
		}

		if(Input.isKeyPressed(Key.R))
			::reloadScript(gameObject, "Reloader")
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
