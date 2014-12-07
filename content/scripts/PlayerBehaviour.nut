dofile("../content/scripts/BaseBehaviour.nut", true);

class PlayerBehaviour extends BaseBehaviourScript </ type = "PlayerBehaviour" />
{	
	constructor(attachedObj)
	{
		base.constructor(attachedObj);
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
			::attachScript(testobj, "Test");
		}

		if(Input.isKeyReleased(Key.B))
		{
			local test = SceneManager.findByName("TestOBJ");
			// local light = CompManager.addLight(test, "TestOBJLight");
			// Light.setColor(light, Vec3(0, 1, 0));
			// GO.attachScript(test, "Debug");
			// GO.reloadAllScriptsByName("TestOBJ");
			//GO.reloadAllScripts(test);
			//GO.reloadScriptType("Test")
			
			local script = ::getScript(test, "Test");
			script.speed = 50.0;
		}

		if(Input.isKeyReleased(Key.R))
		{
			::reloadScript("Player", "PlayerBehaviour");
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
