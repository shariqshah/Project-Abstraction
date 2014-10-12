#ifndef TRANSFORM_H
#define TRANSFORM_H

#define GLM_FORCE_RADIANS
#include "../include/glm/gtx/transform.hpp"
#include "../include/glm/gtx/quaternion.hpp"

#include "component.h"
#include "renderer.h"

class Transform : public Component
{
    glm::vec3 mPosition;
    glm::vec3 mScale;
    glm::vec3 mLookAt;
    glm::vec3 mUp;
    glm::vec3 mForward;
    glm::quat mRotation;
	glm::mat4 mTransformMatrix;
	Node      mNode;

	void updateTransformMatrix();
    void updateLookAt();
    void updateUpVector();
    void updateForward();

	static const float epsilon;
public:
    enum class Space
    {
        LOCAL = 0,
        WORLD
    };
	
    Transform(Node node);
    Transform(Node node,
			  glm::vec3 position,
              glm::vec3 scale,
              glm::quat rotation,
              glm::vec3 lookAt,
              glm::vec3 up,
			  glm::vec3 forward);

    void setPosition(glm::vec3 position, bool updateTransMat = true);
    void translate(glm::vec3 offset, Space transformSpace = Space::WORLD);
    void rotate(glm::vec3 axis, float angle, Space transformSpace = Space::WORLD);
    void setScale(glm::vec3 newScale, bool updateTransMat = true);
    void setLookAt(glm::vec3 lookAt);
    void setUpVector(glm::vec3 up);
    void setRotation(glm::quat newRotation, bool updateTransMat = true);
    void setForward(glm::vec3 direction);
	
    glm::vec3 getPosition();
    glm::vec3 getScale();
    glm::vec3 getLookAt();
    glm::vec3 getUpVector();
    glm::quat getRotation();
	glm::vec3 getRotationVector();
    glm::vec3 getForward();

	virtual const std::string getName();

	static const std::string sName;
	static const glm::vec3   UNIT_X;
	static const glm::vec3   UNIT_Y;
	static const glm::vec3   UNIT_Z;
};

#endif // TRANSFORM_H
