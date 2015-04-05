#include "BaseScript.as"

class FreeCamera : BaseScript
{
	float maxUpDownRot   = 80.f;
	float totalUpDownRot = 0.f;
	float movSpeed       = 20.f;
	float rotSpeed       = 0.5f;
	float sprintFactor   = 3.f;
	Vec3  translation(0.f);
	
	FreeCamera(int32 node)
	{
		super(node);
	}

	protected void update(float deltaTime)
	{
		float increment    = movSpeed * deltaTime;
		float upDownRot    = 0.f; 
		float leftRightRot = 0.f;
		translation.x = translation.y = translation.z = 0.f;

		if(Input::isPressed(MouseButton::RIGHT))
		{
			Input::showCursor(false);
			Input::lockCursor(true);
			
			upDownRot    = Input::getMouseRelY() * rotSpeed;
			leftRightRot = Input::getMouseRelX() * rotSpeed;
		}
		if(Input::isReleased(MouseButton::RIGHT))
		{
			Input::showCursor(true);
			Input::lockCursor(false);
		}		
		if(Input::isPressed(Key::W))      translation.z -= increment;
		if(Input::isPressed(Key::S))	  translation.z += increment;
		if(Input::isPressed(Key::A))	  translation.x -= increment;
		if(Input::isPressed(Key::D))	  translation.x += increment;
		if(Input::isPressed(Key::Q))	  translation.y += increment;
		if(Input::isPressed(Key::E))	  translation.y -= increment;
		if(Input::isPressed(Key::LSHIFT)) translation *= sprintFactor;

		if(Input::isPressed(Key::J)) leftRightRot -= rotSpeed;
		if(Input::isPressed(Key::L)) leftRightRot += rotSpeed;
		if(Input::isPressed(Key::I)) upDownRot -= rotSpeed;
		if(Input::isPressed(Key::K)) upDownRot += rotSpeed;
		
		totalUpDownRot += upDownRot;
		if(totalUpDownRot >= maxUpDownRot)
		{
			totalUpDownRot = maxUpDownRot;
			upDownRot = 0.f;
		}
		else if(totalUpDownRot <= -maxUpDownRot)
		{
			totalUpDownRot = -maxUpDownRot;
			upDownRot = 0.f;
		}
		if(upDownRot != 0.f)    transform.rotate(-UNIT_X, upDownRot, Space::LOCAL);
		if(leftRightRot != 0.f)	transform.rotate(UNIT_Y, -leftRightRot, Space::WORLD);

		if(translation.x != 0 || translation.y != 0 || translation.z != 0)
		{
			transform.translate(translation, Space::LOCAL);
		}
	}
}