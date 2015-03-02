dofile("../content/scripts/BaseBehaviour.nut", true);

class Debug extends BaseBehaviourScript </ type = "Debug" />
{
	constructor(attachedObj)
	{
		base.constructor(attachedObj);
	}

	function update(deltaTime)
	{
		// local transform = CompManager.getTransform(gameObject);
		// Transform.rotate(transform, Vec3(1, 0, 0), 50 * deltaTime, Space.WORLD);

		if(Input.isKeyReleased(Key.V))
		{
			local model = getModel()
			local val = model.materialUniforms.specularStrength
			model.materialUniforms.specularStrength = 10
			Log.message("Specularstrength : " + model.materialUniforms.specularStrength)
		}

		if(Input.isKeyReleased(Key.M))
		{
			local model = getModel()
			local mat = Mat_Type.PHONG
			if(Input.isKeyPressed(Key.LSHIFT))
				mat = Mat_Type.PHONG_TEXTURED
			else if(Input.isKeyPressed(Key.RCTRL))
				mat = Mat_Type.UNSHADED_TEXTURED
			Model.setMaterialType(model, mat)
		}
	}
}

function init(gameObject)
{
	if(!gameObject)
	{
		Log.error("Debug Init", "Invalid gameObject!");
		return null;
	}
	else
	{
		return Debug(gameObject);
	}
}

return init;
