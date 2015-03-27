#include <vector>

#include "transform.h"
#include "scriptengine.h"
#include "gameobject.h"
#include "passert.h"

namespace Transform
{
	namespace
	{
		const float epsilon = 0.005f;
		std::vector<CTransform>   transformList;
		std::vector<unsigned int> emptyIndices;
	}

	int create(Node node)
	{
		CTransform newTransform;
		newTransform.node = node;
		Transform::updateTransformMatrix(&newTransform);
		int index = 0;	
		if(emptyIndices.empty())
		{
			transformList.push_back(newTransform);
			index = transformList.size() - 1;
		}
		else
		{
			index = emptyIndices.back();
			transformList[index] = newTransform;
			emptyIndices.pop_back();
		}
		return index;
	}

	void cleanup()
	{
		transformList.clear();
		emptyIndices.clear();
	}

	CTransform* getTransformAtIndex(int transformIndex)
	{
		CTransform* transform = NULL;
		if(transformIndex >= 0 && transformIndex < (int)transformList.size())
			transform = &transformList[transformIndex];
		return transform;
	}

	bool remove(unsigned int transformIndex)
	{
		bool alreadyRemoved = true;
		for(unsigned int i = 0; i < emptyIndices.size(); i++)
		{
			if(emptyIndices[i] == transformIndex)
			{
				alreadyRemoved = true;
				break;
			}
		}
		if(!alreadyRemoved)
			emptyIndices.push_back(transformIndex);
		else
			Log::warning("Transform at index " + std::to_string(transformIndex) + " already removed!");
		return alreadyRemoved ? false : true;
	}

	void updateTransformMatrix(CTransform* transform)
	{
		Mat4 translationMat   = glm::translate(Mat4(1.0f), transform->position);
		Mat4 scaleMat         = glm::scale(Mat4(1.0f), transform->scale);
		Mat4 rotationMat      = glm::toMat4(glm::normalize(transform->rotation));
		transform->transMat   = translationMat * rotationMat * scaleMat;
		transform->isModified = true;
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
		PA_ASSERT(transform);
		transform->position = position;
		if(updateTransMat) updateTransformMatrix(transform);
	}

	void translate(CTransform* transform, Vec3 offset, Space transformSpace)
	{
		PA_ASSERT(transform);
		if(transformSpace == Space::TS_LOCAL)
			offset = transform->rotation * offset;
		transform->position += offset;
		updateLookAt(transform);
		updateTransformMatrix(transform);
	}

	void setForward(CTransform* transform, Vec3 direction)
	{
		PA_ASSERT(transform);
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
		PA_ASSERT(transform);
		angle = glm::radians(angle);
		if(transformSpace == Space::TS_LOCAL)
			transform->rotation *= glm::normalize(glm::angleAxis(angle, axis));
		else
			transform->rotation  = glm::normalize(glm::angleAxis(angle, axis)) * transform->rotation;
		updateUpVector(transform);
		updateLookAt(transform);
		updateForward(transform);
		updateTransformMatrix(transform);
	}

	void setScale(CTransform* transform, Vec3 newScale, bool updateTransMat)
	{
		PA_ASSERT(transform);
		transform->scale = newScale;
		if(updateTransMat) updateTransformMatrix(transform);
	}

	void setLookAt(CTransform* transform, Vec3 lookAt)
	{
		PA_ASSERT(transform);
		Vec3 direction = lookAt - transform->position;	
		setForward(transform, glm::normalize(direction));
	}

	void setUpVector(CTransform* transform, Vec3 up)
	{
		PA_ASSERT(transform);
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
		PA_ASSERT(transform);
		transform->rotation = newRotation;
		updateUpVector(transform);
		updateLookAt(transform);
		updateForward(transform);
		if(updateTransMat) updateTransformMatrix(transform);
	}

	void resetAllTransformFlags()
	{
		for(CTransform& transform : transformList)
			transform.isModified = false;
	}

	void generateBindings()
	{
		asIScriptEngine* engine = ScriptEngine::getEngine();
		engine->RegisterEnum("Space");
		int rc = engine->RegisterEnumValue("Space", "LOCAL", (int)TS_LOCAL);
		rc = engine->RegisterEnumValue("Space", "WORLD", (int)TS_WORLD);
		
		engine->RegisterObjectType("Transform", sizeof(CTransform), asOBJ_REF | asOBJ_NOCOUNT); PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Transform", "Vec3 position", asOFFSET(CTransform, position));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Transform", "Vec3 forward", asOFFSET(CTransform, forward));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Transform", "Vec3 scale", asOFFSET(CTransform, scale));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Transform", "Quat rotation", asOFFSET(CTransform, rotation));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Transform", "Vec3 lookAt", asOFFSET(CTransform, lookAt));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Transform", "Vec3 up", asOFFSET(CTransform, up));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Transform",
										  "void translate(Vec3, Space = Space::WORLD)",
										  asFUNCTION(translate),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Transform",
										  "void rotate(Vec3, float, Space = Space::WORLD)",
										  asFUNCTION(rotate),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Transform",
										  "void setScale(Vec3, bool = true)",
										  asFUNCTION(setScale),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Transform",
										  "void setRotation(Quat, bool = true)",
										  asFUNCTION(setRotation),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Transform",
										  "void setPosition(Vec3, bool = true)",
										  asFUNCTION(setPosition),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Transform",
										  "void setLookAt(Vec3)",
										  asFUNCTION(setLookAt),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Transform",
										  "void setUp(Vec3)",
										  asFUNCTION(setUpVector),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Transform",
										  "void setForward(Vec3)",
										  asFUNCTION(setForward),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Transform",
										  "void updateTransformMatrix()",
										  asFUNCTION(updateTransformMatrix),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);		
		rc = engine->RegisterGlobalProperty("const Vec3 UNIT_X", (void *)&UNIT_X); PA_ASSERT( rc >= 0 );
		rc = engine->RegisterGlobalProperty("const Vec3 UNIT_Y", (void *)&UNIT_Y); PA_ASSERT( rc >= 0 );
		rc = engine->RegisterGlobalProperty("const Vec3 UNIT_Z", (void *)&UNIT_Z); PA_ASSERT( rc >= 0 );
	}
}
