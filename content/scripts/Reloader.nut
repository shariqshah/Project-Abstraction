dofile("../content/scripts/BaseBehaviour.nut", true);

class Reloader extends BaseBehaviourScript </ type = "Reloader" />
{
	speed = null
	count = null
	model = null
	color = null
	
	constructor(attachedObj)
	{
		base.constructor(attachedObj)
		speed = 50.0
		count = 0

		color = Vec4(0.5, 0.5, 0, 1)
		model = CModel()
		Model.loadFromFile("suzanne.pamesh", model)
	}

	function update(deltaTime)
	{
		if(Input.isKeyReleased(Key.C))
		{
			local player = SceneManager.findByName("Debug")
			::reloadScript(player, "Debug")
		}

		if(Input.isKeyReleased(Key.R))
			::reloadScript(getGameObject(), "Reloader")

		if(Input.isKeyReleased(Key.F))
		{
			count++
			local temp = SceneManager.create("Test" + count)

			local model = GO.addModel(temp, "suzanne.pamesh")
			model.materialUniforms.diffuseColor = color
			model.materialUniforms.texture = Texture.create("test2.png")
			if(Input.isKeyPressed(Key.LCTRL))
				Model.setMaterialType(model, Mat_Type.PHONG)
			
			local player = SceneManager.findByName("Player")
			local pt = GO.getTransform(player)
			local t = GO.getTransform(temp)
			Transform.setPosition(t, pt.position, true)
			// local light = GO.addLight(temp)
			// light.lightType  = LightType.POINT
			// light.color = color
		}
	}
}

function init(gameObject)
{
	if(!gameObject)
	{
		Log.error("Reloader Init", "Invalid gameObject!");
		return null;
	}
	else
	{
		return Reloader(gameObject);
	}
}

return init;
