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
		Transform.rotate(transform, Vec3(0, 1, 0), 10 * deltaTime, Space.WORLD);
		//Log.message("Test is alive!");

		// if(Input.isKeyReleased(Key.V))
		// {
		// 	Log.message("Hi");
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
