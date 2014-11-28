class BaseBehaviourScript
{
	gameObject = null;
	type       = null;
	
	constructor(attachedObj, typeName)
	{
		assert(attachedObj != null);
		
		gameObject = attachedObj;
		type       = typeName;
	}

	function start()
	{
		Log.message("Base start called!");
	}
}
