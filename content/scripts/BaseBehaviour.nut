class BaseBehaviourScript </ type = "BaseBehaviourScript" />
{
	gameObject = null;
	
	constructor(attachedObj)
	{
		assert(attachedObj != null);		
		gameObject = attachedObj;
	}

	function update(deltaTime)
	{
		// To be overridden by extending class
	}

	function onCollision(collisionData)
	{
		// To be overridden by extending class
	}
}
