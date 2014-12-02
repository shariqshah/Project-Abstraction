dofile("../content/scripts/BaseBehaviour.nut", true);

class PlayerBehaviour extends BaseBehaviourScript
{	
	constructor(attachedObj)
	{
		base.constructor(attachedObj, "PlayerBehaviour");
	}

	function update(deltaTime)
	{
		if(Input.isKeyReleased(Key.V))
		{
			SceneManager.removeByName("TestOBJ");			
		}
	}
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
