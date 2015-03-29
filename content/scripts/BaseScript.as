class BaseScript : IScriptable
{
	protected int32 node; 				// Cached value of the gameoject this script is attached to

	protected GameObject@ gameobject
	{
		get
		{
			return SceneManager::find(node);
		}
	}

	protected Transform@ transform
	{
		get
		{
			return gameobject.getTransform();
		}
	}

	protected Light@ light
	{
		get
		{
			return gameobject.getLight();
		}
	}

	protected Model@ model
	{
		get
		{
			return gameobject.getModel();
		}
	}

	protected Camera@ camera
	{
		get
		{
			return gameobject.getCamera();
		}
	}
	
	BaseScript(int32 node)
	{
		if(node >= 0) this.node = node;
		else Log::error("BaseScript", "Invalid node");
	}

	protected void update(float deltaTime)
	{
		// For child scripts to fill in
	}

	protected void onCollision(const CollisionData@ collisionData)
	{
		// For child script to fill in
		Log::message("BASE");
	}
}
