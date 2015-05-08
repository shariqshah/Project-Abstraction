#ifndef _light_H
#define _light_H

#include <vector>
#include "mathdefs.h"
#include "componentTypes.h"
#include "datatypes.h"
#include "boundingvolumes.h"
#include "jsondefs.h"

#define MAX_SHADOWMAPS 4

enum LightType
{
	LT_SPOT  = 0,
	LT_DIR,
	LT_POINT
};

struct CLight
{
	float          outerAngle   = glm::radians(30.f);
	float          innerAngle   = glm::radians(20.f);
	float          falloff      = 1.5f;
	float          intensity    = 1.0f;
	Vec4           color        = Vec4(1.f);
	Node           node         = 0;
	bool           castShadow   = false;
	bool           pcfEnabled   = false;
	bool           valid        = true;
	int            type         = LT_POINT;
	int            radius       = 30;
	int            shadowMap[4] = {-1, -1, -1, -1};
	float          depthBias    = 0.0005f;
	BoundingSphere boundingSphere;
};

namespace Light
{
	const static int MAX_LIGHTS = 32;
	int                    create(Node node);
	bool                   createFromJSON(CLight* light, const rapidjson::Value& value);
	void                   initialize();
	void                   generateBindings();		
	void                   cleanup();
	void                   remove(uint32_t index);
	CLight*                getLightAtIndex(uint32_t index);
	void                   setRadius(CLight* light, float radius);
	void                   setType(CLight* light, LightType type);
	void                   setOuterAngle(CLight* light, float outerAngle);
	void                   setInnerAngle(CLight* light, float innerAngle);
	void                   setFalloff(CLight* light, float falloff);
	void                   setIntensity(CLight* light, float intensity);
	void                   setCastShadow(CLight* light, bool castShadow);
	bool                   writeToJSON(CLight* light, rapidjson::Writer<rapidjson::StringBuffer>& writer);
	std::vector<uint32_t>* getActiveLights();
}


#endif
