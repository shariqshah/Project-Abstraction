dofile("../content/scripts/BaseBehaviour.nut", true);

class PlayerBehaviour extends BaseBehaviourScript
{	
	constructor(attachedObj)
	{
		base.constructor(attachedObj, "PlayerBehaviour");
	}

	function update(deltaTime)
	{
		// local transform = CompManager.getTransform(gameObject);
		// Transform.translate(transform, Vec3(0, 0, -5 * deltaTime), Space.LOCAL);

		if(Input.isKeyReleased(Key.V))
		{
			local light = CompManager.getLight(gameObject);
			Light.setColor(light, Vec3(0, 1, 1));
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
		//addScript(player, PlayerBehaviour(gameObject));
		return PlayerBehaviour(gameObject);
	}
}

return init;
