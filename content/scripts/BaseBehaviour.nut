class BaseBehaviourScript </ type = "BaseBehaviourScript" />
{
	gameObject = null;
	
	constructor(attachedObj)
	{
		assert(attachedObj != null);
		
		gameObject = attachedObj;
	}
}
