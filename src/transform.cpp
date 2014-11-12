#include "transform.h"
#include "scriptengine.h"

namespace Transform
{
	namespace
	{
		static const float epsilon = 0.005f;
	}

	void updateTransformMatrix(CTransform* transform)
	{
		Mat4 translationMat = glm::translate(Mat4(1.0f), transform->position);
		Mat4 scaleMat       = glm::scale(Mat4(1.0f), transform->scale);
		Mat4 rotationMat    = glm::toMat4(glm::normalize(transform->rotation));

		transform->transMat = translationMat * rotationMat * scaleMat;
		Renderer::setNodeTransform(transform->node, transform->transMat);
	}

	void updateLookAt(CTransform* transform)
	{
		Vec3 newLookAt = transform->rotation * -UNIT_Z;
		transform->lookAt = transform->position + newLookAt;
	}

	void updateUpVector(CTransform* transform)
	{
		transform->up = glm::normalize(transform->rotation * UNIT_Y);
	}

	void updateForward(CTransform* transform)
	{
		transform->forward = glm::normalize(transform->rotation * -UNIT_Z);
	}

	void setPosition(CTransform* transform, Vec3 position, bool updateTransMat)
	{
		transform->position = position;

		if(updateTransMat)
			updateTransformMatrix(transform);
	}

	void translate(CTransform* transform, Vec3 offset, Space transformSpace)
	{
		if(transformSpace == Space::LOCAL)
			offset = transform->rotation * offset;
		
		transform->position += offset;

		updateLookAt(transform);
		updateTransformMatrix(transform);
	}

	void setForward(CTransform* transform, Vec3 direction)
	{
		//TODO: Fix this function by comparing with jDoom
		Vec3  newForward = glm::normalize(direction);
		float angle      = glm::dot(transform->forward, newForward);
	
		//angle = glm::clamp(angle, -1.f, 1.f);
		angle = glm::acos(angle);
		angle = glm::degrees(-angle);

		if(angle > epsilon || angle < -epsilon)
		{
			Vec3 rotationAxis = glm::cross(newForward, transform->forward);
			rotate(transform, glm::normalize(rotationAxis), angle);
		}
	}

	void rotate(CTransform* transform, Vec3 axis, float angle, Space transformSpace)
	{
		angle = glm::radians(angle);
		if(transformSpace == Space::LOCAL)
			transform->rotation *= glm::normalize(glm::angleAxis(angle, axis));
		else
			transform->rotation  = glm::normalize(glm::angleAxis(angle, axis))*
				                   transform->rotation;

		updateUpVector(transform);
		updateLookAt(transform);
		updateForward(transform);

		updateTransformMatrix(transform);
	}

	void setScale(CTransform* transform, Vec3 newScale, bool updateTransMat)
	{
		transform->scale = newScale;

		if(updateTransMat)
			updateTransformMatrix(transform);
	}

	void setLookAt(CTransform* transform, Vec3 lookAt)
	{
		Vec3 direction = lookAt - transform->position;	
		setForward(transform, glm::normalize(direction));
	}

	void setUpVector(CTransform* transform, Vec3 up)
	{
		Vec3  newUp = glm::normalize(up);
		float angle = glm::dot(transform->up, newUp);

		angle = glm::acos(angle);
		angle = glm::degrees(-angle);
	
		if(angle > epsilon || angle < -epsilon)
		{
			Vec3 rotationAxis = glm::cross(newUp, transform->up);
			rotate(transform, glm::normalize(rotationAxis), angle);
		}
	}

	void setRotation(CTransform* transform, Quat newRotation, bool updateTransMat)
	{
		transform->rotation = newRotation;

		updateUpVector(transform);
		updateLookAt(transform);
		updateForward(transform);

		if(updateTransMat)
			updateTransformMatrix(transform);
	}

	void resetTransformFlag(CTransform* transform)
	{
		Renderer::resetTransformFlag(transform->node);
	}

	void initialize()
	{
		Sqrat::RootTable().Bind("Vec3", Sqrat::Class<glm::vec3>()
								.Var("x", &glm::vec3::x)
								.Var("y", &glm::vec3::y)
								.Var("z", &glm::vec3::z)
								.Ctor()
								.Ctor<float>()
								.Ctor<int>()
								.Ctor<int, int, int>()
								.Ctor<float, float, float>());

		Sqrat::RootTable().Bind("Quat", Sqrat::Class<glm::quat>()
								.Var("x", &glm::quat::x)
								.Var("y", &glm::quat::y)
								.Var("z", &glm::quat::z)
								.Var("w", &glm::quat::w));
		
		Sqrat::RootTable().Bind("CTransform", Sqrat::Class<CTransform>()
								.Var("position", &CTransform::position)
								.Var("scale",    &CTransform::scale)
								.Var("rotation", &CTransform::rotation)
								.Var("lookAt",   &CTransform::lookAt)
								.Var("up",       &CTransform::up)
								.Var("forawrd",  &CTransform::forward));

		Sqrat::ConstTable().Enum("Space", Sqrat::Enumeration ()
								 .Const("LOCAL", 0)
								 .Const("WORLD", 1));

		Sqrat::RootTable().Bind("Transform", Sqrat::Table(ScriptEngine::getVM())
								.Func("translate", &translate)
								.Func("rotate", &rotate)
								.Func("scale", &setScale)
								.Func("setLookAt", &setLookAt)
								.Func("setRotation", &setRotation)
								.Func("setUpVector", &setUpVector)
								.Func("setForward", &setForward)
								.Func("resetTransformFlag", &resetTransformFlag)
								.Func("updateTransformMatrix", &updateTransformMatrix));
	}
}
