#ifndef _light_H
#define _light_H

#include "renderer.h"
#include "component.h"

class Light : public Component
{
	float     mRadius;
	float     mFov;
	float     mIntensity;
	float     mShadowMapBias;
	float     mShadowSplitLambda;
	int       mShadowMapCount;
	glm::vec3 mColor;
	Node      mNode;

public:	
	Light(Node parent, std::string name);

	float     getRadius();
	float     getFov();
	float     getIntensity();
	float     getShadowMapBias();
	float     getShadowSplitLambda();
	int       getShadowMapCount();
	glm::vec3 getColor();
	Node      getNode();
	
	void setRadius(float radius);
	void setColor(glm::vec3 color);
	void setFov(float fov);
	void setIntensity(float intensity);
	void setShadowMapBias(float bias);
	void setShadowSplitLambda(float splitLambda);
	void setShadowMapCount(int shadowMapCount); 
};

#endif
