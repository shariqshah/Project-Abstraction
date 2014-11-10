#ifndef _light_H_
#define _light_H_

#include "renderer.h"
#include "componentTypes.h"

struct CLight
{
	float radius            = 75.f;
	float fov               = 90.f;
	float intensity         = 1.f;
	float shadowmapBias     = 0.005f;
	float shadowSplitLambda = 0.9f;
	int   shadowmapCount    = 1;
	Vec3  color             = Vec3(1.f);
	Node  node              = 0;
	bool  castShadow        = false;
	bool  valid             = false;
};

namespace Renderer
{
	namespace Light
	{
		CLight create(Node parent,
					  const std::string& name,
					  const std::string& lightContext  = "LIGHTING",
					  const std::string& shadowContext = "SHADOWMAP");
		void setRadius(CLight* light, float radius);
		void setColor(CLight* light, Vec3 color);
		void setFov(CLight* light, float fov);
		void setIntensity(CLight* light, float intensity);
		void setShadowMapBias(CLight* light, float bias);
		void setShadowSplitLambda(CLight* light, float splitLambda);
		void setShadowMapCount(CLight* light, int shadowMapCount);
		void setShadowCaster(CLight* light, bool enable);
	}
}


#endif
