#include "light.h"

namespace Renderer
{
	namespace Light
	{
		CLight create(Node parent,
					  const std::string& name,
					  const std::string& lightContext,
					  const std::string& shadowContext)
		{
			//TODO: Fix light material related parameter
			CLight newLight;
			Resource lightMat = Renderer::Resources::get(ResourceType::MATERIAL,
														 "materials/light.material.xml");
			newLight.node = h3dAddLightNode(parent,
											name.c_str(),
											lightMat,
											lightContext.c_str(),
											shadowContext.c_str());

			if(newLight.node == 0)
				Log::error("Light", name + " node not created!");
			else
			{
				setRadius(&newLight, newLight.radius);
				setColor(&newLight, newLight.color);
				setFov(&newLight, newLight.fov);
				setShadowMapCount(&newLight, newLight.shadowmapCount);
				setShadowMapBias(&newLight, newLight.shadowmapBias);
				setShadowSplitLambda(&newLight, newLight.shadowSplitLambda);
				setShadowCaster(&newLight, newLight.castShadow);
				setIntensity(&newLight, newLight.intensity);
				newLight.valid = true;
			}

			return newLight;
		}
		
		void setRadius(CLight* light, float radius)
		{
			light->radius = radius;
			h3dSetNodeParamF(light->node, H3DLight::RadiusF, 0, radius);
		}
		
		void setColor(CLight* light, Vec3 color)
		{
			light->color = color;
			h3dSetNodeParamF(light->node, H3DLight::ColorF3, 0, color.r);
			h3dSetNodeParamF(light->node, H3DLight::ColorF3, 1, color.g);
			h3dSetNodeParamF(light->node, H3DLight::ColorF3, 2, color.b);
		}
		
		void setFov(CLight* light, float fov)
		{
			light->fov = fov;
			h3dSetNodeParamF(light->node, H3DLight::FovF, 0, fov);
		}

		void setShadowCaster(CLight* light, bool enable)
		{
			if(enable)
			{
				light->castShadow = true;
				setShadowMapCount(light, light->shadowmapCount);
			}
			else
			{
				light->castShadow = false;
				setShadowMapCount(light, 0);
			}
		}
		
		void setShadowMapCount(CLight* light, int count)
		{
			light->shadowmapCount = count;
			
			h3dSetNodeParamI(light->node, H3DLight::ShadowMapCountI, count);

			if(light->shadowmapCount > 0 && !light->castShadow)
				light->castShadow = true;
			else if(light->shadowmapCount == 0 && light->castShadow)
				light->castShadow = false;
		}
		
		void setShadowMapBias(CLight* light, float bias)
		{
			light->shadowmapBias = bias;
			h3dSetNodeParamF(light->node, H3DLight::ShadowMapBiasF, 0, bias);
		}

		void setShadowSplitLambda(CLight* light, float splitLambda)
		{
			light->shadowSplitLambda = splitLambda;
			h3dSetNodeParamF(light->node,
							 H3DLight::ShadowSplitLambdaF,
							 0,
							 splitLambda);
		}
		
		void setIntensity(CLight* light, float intensity)
		{
			light->intensity = intensity;
			h3dSetNodeParamF(light->node, H3DLight::ColorMultiplierF, 0, intensity);
		}
	}

}
