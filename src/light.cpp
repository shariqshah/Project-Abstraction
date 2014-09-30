#include "light.h"

Light::Light(Node parent, std::string name)
{
	mType = ComponentType::LIGHT;
	mName = "Light";
	mNode = Renderer::Light::create(parent, name);

	if(mNode == 0)
		mValid = false;
	else
	{
		setRadius(100.f);
		setColor(glm::vec3(1.f));             
		setFov(90.f);                     
		setIntensity(1.f);         
		setShadowMapBias(0.005f);          
		setShadowSplitLambda(0.5f);
		setShadowMapCount(1);
	}
}

Light::~Light()
{
	if(!Renderer::removeNode(mNode))
		Log::warning("Light node does not exist so not removed!");
}

float Light::getRadius()
{
	return mRadius;
}

float Light::getFov()
{
	return mFov;
}

float Light::getIntensity()
{
	return mIntensity;
}

float Light::getShadowMapBias()
{
	return mShadowMapBias;
}

float Light::getShadowSplitLambda()
{
	return mShadowSplitLambda;
}

int Light::getShadowMapCount()
{
	return mShadowMapCount;
}

glm::vec3 Light::getColor()
{
	return mColor;
}
	
void Light::setRadius(float radius)
{
	mRadius = radius;
	Renderer::Light::setRadius(mNode, mRadius);
}

void Light::setColor(glm::vec3 color)
{
	mColor = color;
	Renderer::Light::setColor(mNode, mColor);
}

void Light::setFov(float fov)
{
	mFov = fov;
	Renderer::Light::setFov(mNode, mFov);
}

void Light::setIntensity(float intensity)
{
	mIntensity = intensity;
	Renderer::Light::setIntensity(mNode, mIntensity);
}

void Light::setShadowMapBias(float bias)
{
	mShadowMapBias = bias;
	Renderer::Light::setShadowMapBias(mNode, mShadowMapBias);
}

void Light::setShadowSplitLambda(float splitLambda)
{
	mShadowSplitLambda = splitLambda;
	Renderer::Light::setShadowSplitLambda(mNode, mShadowSplitLambda);
}

void Light::setShadowMapCount(int shadowMapCount)
{
	mShadowMapCount = shadowMapCount;
	Renderer::Light::setShadowMapCount(mNode, mShadowMapCount);
}
