#include "transform.h"

const std::string Transform::sName = "Transform";

Transform::Transform() : Transform(glm::vec3(0.f, 0.f, 15.f),
                                   glm::vec3(1.0f),
                                   glm::quat(),
                                   glm::vec3(0.f, 0.f, -5.f),
                                   glm::vec3(0.f, 1.f, 0.f),
                                   glm::vec3(0.f, 0.f, -1.f))
{}


Transform::Transform(glm::vec3 position,
                     glm::vec3 scale,
                     glm::quat rotation,
                     glm::vec3 lookAt,
                     glm::vec3 up,
                     glm::vec3 forward)
{
    mPosition = position;
    mScale = scale;
    mRotation = rotation;
    mLookAt = lookAt;
    mUp = up;
    mForward = forward;
    mType = ComponentType::TRANSFORM;
    mNeedsSync = true;
}

void Transform::setPosition(glm::vec3 position)
{
    mPosition = position;
	mNeedsSync = true;
}

void Transform::translate(glm::vec3 offset, Space transformSpace)
{
    if(transformSpace == Space::LOCAL)
    {
        offset = mRotation * offset;
    }
    mPosition += offset;

    updateLookAt();
	mNeedsSync = true;
}

glm::vec3 Transform::getForward()
{
    return mForward;
}

void Transform::setForward(glm::vec3 direction)
{
    glm::vec3 newForward = glm::normalize(direction);
    float angle = glm::dot(mForward, newForward);
    angle = glm::clamp(angle, -1.f, 1.f);
    angle = glm::acos(angle);
    glm::vec3 rotationAxis = glm::cross(newForward, mForward);
    rotate(glm::normalize(rotationAxis), glm::degrees(-angle));
	mNeedsSync = true;
}

void Transform::updateLookAt()
{
    glm::vec3 originalLookAt(0, 0, -1);
    //glm::vec3 newLookAt = originalLookAt * mRotation;
    glm::vec3 newLookAt = mRotation * originalLookAt;
    mLookAt = mPosition + newLookAt;
}

void Transform::updateUpVector()
{
    glm::vec3 originalUp(0, 1, 0);
//    mUp = glm::normalize(originalUp * rotation);
    mUp = glm::normalize(mRotation * originalUp);
}

void Transform::updateForward()
{
    mForward = glm::normalize(mRotation * glm::vec3(0, 0, -1));
}

void Transform::rotate(glm::vec3 axis, float angle, Space transformSpace)
{
    if(transformSpace == Space::LOCAL)
        mRotation *= glm::normalize(glm::angleAxis(glm::radians(angle), axis));
    else
        mRotation = glm::normalize(glm::angleAxis(glm::radians(angle), axis))
                         * mRotation;

    updateUpVector();
    updateLookAt();
    updateForward();

	mNeedsSync = true;
}

void Transform::setScale(glm::vec3 newScale)
{
    mScale = newScale;
	mNeedsSync = true;
}

void Transform::setLookAt(glm::vec3 lookAt)
{
	glm::vec3 direction = lookAt - mPosition;	
	setForward(glm::normalize(direction));
}

void Transform::setUpVector(glm::vec3 up)
{
    mUp = up;
	mNeedsSync = true;
}

void Transform::setRotation(glm::quat newRotation)
{
    mRotation = newRotation;

    updateUpVector();
    updateLookAt();
    updateForward();

	mNeedsSync = true;
}

glm::mat4 Transform::getModelMatrix()
{
    glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), mPosition);
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), mScale);
    glm::mat4 rotationMat = glm::toMat4(glm::normalize(mRotation));

    glm::mat4 ModelMat = translationMat * rotationMat * scaleMat;

    return ModelMat;
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

bool Transform::needsSync()
{
	return mNeedsSync;
}

void Transform::setSynced()
{
	mNeedsSync = false;
}

const std::string Transform::getName()
{
	return sName;
}
