dofile("../content/scripts/BaseBehaviour.nut", true);

class Test extends BaseBehaviourScript </ type = "Test" />
{
	speed = null;
	
	constructor(attachedObj)
	{
		base.constructor(attachedObj);
		speed = 50.0;
	}

	function update(deltaTime)
	{
		local transform = GO.getTransform(gameObject);
		Transform.rotate(transform, Vec3(0, 1, 0), speed * deltaTime, Space.WORLD);

		// if(Input.isKeyReleased(Key.V))
		// {
		// 	local player = SceneManager.findByName("Player")
		// 	this.reloadScriptByName(player, "PlayerBehaviour")
		// }
	}

	function onCollision(collisionData)
	{
		//Log.message(gameObject.name + " collided with " + collisionData.collidingObj.name)
		local obj = collisionData.collidingObj;

		if(obj.tag == "child")
			SceneManager.removeByNode(obj.node)
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
