dofile("../content/scripts/BaseBehaviour.nut", true);

class Test extends BaseBehaviourScript
{
	constructor(attachedObj)
	{
		base.constructor(attachedObj, "Test");
	}

	function update(deltaTime)
	{
		local transform = CompManager.getTransform(gameObject);
		Transform.rotate(transform, Vec3(0, 1, 0), 50 * deltaTime, Space.WORLD);

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
		Log.error("Test Init", "Invalid gameObject!");
		return null;
	}
	else
	{
		return Test(gameObject);
	}
}

return init;
