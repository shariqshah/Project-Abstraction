#include "transform.h"

const std::string Transform::sName   = "Transform";
const glm::vec3   Transform::UNIT_X  = glm::vec3(1, 0, 0);
const glm::vec3   Transform::UNIT_Y  = glm::vec3(0, 1, 0);
const glm::vec3   Transform::UNIT_Z  = glm::vec3(0, 0, 1);
const float       Transform::epsilon = 0.005;

Transform::Transform(Node node) : Transform(node,
											glm::vec3(0.f, 0.f, 15.f),
											glm::vec3(1.0f),
											glm::quat(),
											glm::vec3(0.f, 0.f, -5.f),
											glm::vec3(0.f, 1.f, 0.f),
											glm::vec3(0.f, 0.f, -1.f))
{}


Transform::Transform(Node node,
					 glm::vec3 position,
                     glm::vec3 scale,
                     glm::quat rotation,
                     glm::vec3 lookAt,
                     glm::vec3 up,
                     glm::vec3 forward)
{
	mNode = node;
    mPosition = position;
    mScale = scale;
    mRotation = rotation;
    mLookAt = lookAt;
    mUp = up;
    mForward = forward;
    mType = ComponentType::TRANSFORM;

	updateTransformMatrix();
}

void Transform::setPosition(glm::vec3 position, bool updateTransMat)
{
    mPosition = position;

	if(updateTransMat)
		updateTransformMatrix();
}

void Transform::translate(glm::vec3 offset, Space transformSpace)
{
    if(transformSpace == Space::LOCAL)
    {
        offset = mRotation * offset;
    }
    mPosition += offset;

    updateLookAt();
	updateTransformMatrix();
}

glm::vec3 Transform::getForward()
{
    return mForward;
}

void Transform::setForward(glm::vec3 direction)
{
	//TODO: Fix this function by comparing with jDoom
    glm::vec3 newForward = glm::normalize(direction);
    float     angle      = glm::dot(mForward, newForward);
	
    //angle = glm::clamp(angle, -1.f, 1.f);
    angle = glm::acos(angle);
	angle = glm::degrees(-angle);

	if(angle > epsilon || angle < -epsilon)
	{
		glm::vec3 rotationAxis = glm::cross(newForward, mForward);
		rotate(glm::normalize(rotationAxis), angle);
	}
}

void Transform::updateLookAt()
{
    glm::vec3 newLookAt = mRotation * -UNIT_Z;
    mLookAt = mPosition + newLookAt;
}

void Transform::updateUpVector()
{
    mUp = glm::normalize(mRotation * UNIT_Y);
}

void Transform::updateForward()
{
    mForward = glm::normalize(mRotation * -UNIT_Z);
}

void Transform::rotate(glm::vec3 axis, float angle, Space transformSpace)
{
	angle = glm::radians(angle);
    if(transformSpace == Space::LOCAL)
        mRotation *= glm::normalize(glm::angleAxis(angle, axis));
    else
        mRotation  = glm::normalize(glm::angleAxis(angle, axis))
                         * mRotation;

    updateUpVector();
    updateLookAt();
    updateForward();

	updateTransformMatrix();
}

void Transform::setScale(glm::vec3 newScale, bool updateTransMat)
{
    mScale = newScale;

	if(updateTransMat)
		updateTransformMatrix();
}

void Transform::setLookAt(glm::vec3 lookAt)
{
	glm::vec3 direction = lookAt - mPosition;	
	setForward(glm::normalize(direction));
}

void Transform::setUpVector(glm::vec3 up)
{
    glm::vec3 newUp = glm::normalize(up);
    float     angle = glm::dot(mUp, newUp);

    angle = glm::acos(angle);
	angle = glm::degrees(-angle);
	
	if(angle > epsilon || angle < -epsilon)
	{
		glm::vec3 rotationAxis = glm::cross(newUp, mUp);
		rotate(glm::normalize(rotationAxis), angle);
	}
}

void Transform::setRotation(glm::quat newRotation, bool updateTransMat)
{
    mRotation = newRotation;

    updateUpVector();
    updateLookAt();
    updateForward();

	if(updateTransMat)
		updateTransformMatrix();
}

void Transform::updateTransformMatrix()
{
    glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), mPosition);
    glm::mat4 scaleMat       = glm::scale(glm::mat4(1.0f), mScale);
    glm::mat4 rotationMat    = glm::toMat4(glm::normalize(mRotation));

    mTransformMatrix = translationMat * rotationMat * scaleMat;
	Renderer::setNodeTransform(mNode, mTransformMatrix);
}

glm::vec3 Transform::getPosition()
{
    return mPosition;
}

glm::vec3 Transform::getScale()
{
    return mScale;
}

glm::vec3 Transform::getLookAt()
{
    return mLookAt;
}

glm::vec3 Transform::getUpVector()
{
    return mUp;
}

glm::quat Transform::getRotation()
{
    return mRotation;
}

glm::vec3 Transform::getRotationVector()
{
	return glm::degrees(glm::eulerAngles(mRotation));
}  

const std::string Transform::getName()
{
	return sName;
}
