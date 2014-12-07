dofile("../content/scripts/BaseBehaviour.nut", true);

class PlayerBehaviour extends BaseBehaviourScript </ type = "PlayerBehaviour" />
{	
	constructor(attachedObj)
	{
		base.constructor(attachedObj, "PlayerBehaviour");
		count = 0;
	}

	function update(deltaTime)
	{
		local pTransform = CompManager.getTransform(gameObject);
		// local translation  = Vec3(0.0, 0.0, 0.0);
		// local speed = 5.0;

		// if(Input.isKeyPressed(Key.I))
		// 	translation.x += deltaTime * speed
		//Log.message("Player Alive!")

		if(Input.isKeyReleased(Key.V))
		{
			// local test = SceneManager.findByName("TestOBJ");
			// reloadScriptByName(test, "Test");
			count++;
			local testobj = SceneManager.create("TestOBJ" + count);
			CompManager.addModel(testobj, "models/test/test.scene.xml");
			local transform = CompManager.getTransform(testobj);
			Transform.setPosition(transform, pTransform.position, true);
			GO.attachScript(testobj, "Test");
		}

		if(Input.isKeyReleased(Key.B))
		{
			GO.reloadScriptType("Test");
		}
	}

	count = null;
}

function init(gameObject)
{
	if(!gameObject)
	{
		Log.error("PlayerBehaviour Init", "Invalid gameObject!");
		return null;
	}
	else
	{
		return PlayerBehaviour(gameObject);
	}
}

return init;
