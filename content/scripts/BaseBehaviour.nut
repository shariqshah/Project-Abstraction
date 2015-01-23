class BaseBehaviourScript </ type = "BaseBehaviourScript" />
{
	node = null;
	
	constructor(attachedObj)
	{
		assert(attachedObj != null);		
		node = attachedObj.node;
	}

	function update(deltaTime)
	{
		// To be overridden by extending class
	}

	function onCollision(collisionData)
	{
		// To be overridden by extending class
	}

	function getGameObject()
	{
		return SceneManager.findByNode(node)
	}
	
	function getTransform()
	{
		local gameObject = getGameObject()
		return GO.getTransform(gameObject)
	}

	function getModel()
	{
		local gameObject = getGameObject()
		return GO.getModel(gameObject)
	}

	function getCamera()
	{
		local gameObject = getGameObject()
		return GO.getCamera(gameObject)
	}

	function getLight()
	{
		local gameObject = getGameObject()
		return GO.getLight(gameObject)
	}
}
