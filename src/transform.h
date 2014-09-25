#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <memory>

#define GLM_FORCE_RADIANS
#include "../include/glm/glm.hpp"
#include "../include/glm/gtx/transform.hpp"
#include "../include/glm/gtx/quaternion.hpp"
#include "../include/glm/gtc/quaternion.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include "component.h"

class Transform : public Component
{
    glm::vec3 mPosition;
    glm::vec3 mScale;
    glm::vec3 mLookAt;
    glm::vec3 mUp;
    glm::vec3 mForward;
    glm::quat mRotation;
	bool      mNeedsSync;

    void updateLookAt();
    void updateUpVector();
    void updateForward();
public:
    enum class Space
    {
        LOCAL = 0,
        WORLD
    };
	
    Transform();
    Transform(glm::vec3 position,
              glm::vec3 scale,
              glm::quat rotation,
              glm::vec3 lookAt,
              glm::vec3 up,
			  glm::vec3 forward);

    void setPosition(glm::vec3 position);
    void translate(glm::vec3 offset, Space transformSpace = Space::WORLD);
    void rotate(glm::vec3 axis, float angle, Space transformSpace = Space::WORLD);
    void setScale(glm::vec3 newScale);
    void setLookAt(glm::vec3 lookAt);
    void setUpVector(glm::vec3 up);
    void setRotation(glm::quat newRotation);
    void setForward(glm::vec3 direction);
	void setSynced();
	
    glm::vec3 getPosition();
    glm::vec3 getScale();
    glm::vec3 getLookAt();
    glm::vec3 getUpVector();
    glm::quat getRotation();
    glm::vec3 getForward();
	bool      needsSync();
};

#endif // TRANSFORM_H
