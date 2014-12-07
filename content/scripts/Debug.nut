dofile("../content/scripts/BaseBehaviour.nut", true);

class Debug extends BaseBehaviourScript </ type = "Debug" />
{
	constructor(attachedObj)
	{
		base.constructor(attachedObj);
	}

	function update(deltaTime)
	{
		local transform = CompManager.getTransform(gameObject);
		Transform.rotate(transform, Vec3(1, 0, 0), 50 * deltaTime, Space.WORLD);

		// if(Input.isKeyReleased(Key.V))
		// {
		// 	local player = SceneManager.findByName("Player")
		// 	this.reloadScriptByName(player, "PlayerBehaviour")
		// }
	}
}

function init(gameObject)
{
	if(!gameObject)
	{
		Log.error("Debug Init", "Invalid gameObject!");
		return null;
	}
	else
	{
		return Debug(gameObject);
	}
}

return init;
