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
	float       outerAngle  = glm::radians(30.f);
	float       innerAngle  = glm::radians(20.f);
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
	}
}


#endif
