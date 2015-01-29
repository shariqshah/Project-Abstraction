#ifndef _light_H
#define _light_H

#include <vector>
#include "mathdefs.h"
#include "componentTypes.h"

typedef uint32_t Node;

enum LightType
{
	LT_SPOT  = 0,
	LT_DIR,
	LT_POINT
};

struct Attenuation
{
	float constant  = 1.f;
	float linear    = 0.1f;
	float quadratic = 0.001f;
};

struct CLight
{
	float       outerAngle  = 30.f;
	float       innerAngle  = 20.f;
	float       exponent    = 4.f;
	float       intensity   = 1.0f;
	Vec4        color       = Vec4(1.f);
	Node        node        = 0;
	bool        castShadow  = false;
	bool        valid       = true;
	int         type        = LT_SPOT;
	Attenuation attenuation;
};

namespace Renderer
{
	namespace Light
	{
		const int MAX_LIGHTS = 32;
		
		int                    create(Node node);
		void                   initialize();
		void                   generateBindings();		
		void                   cleanup();
		void                   remove(uint32_t index);
		CLight*                getLightAtIndex(uint32_t index);
		std::vector<uint32_t>* getActiveLights();

		
		// void    setRadius(CLight* light, float radius);
		// void    setColor(CLight* light, Vec3 color);
		// void    setFov(CLight* light, float fov);
		// void    setIntensity(CLight* light, float intensity);
		// void    setShadowMapBias(CLight* light, float bias);
		// void    setShadowCaster(CLight* light, bool enable);
		// void    setLightType(CLight* light, LightType type);
	}
}


#endif
