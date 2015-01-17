#include "light.h"
#include "scriptengine.h"

namespace Renderer
{
	namespace
	{
		std::vector<CLight>   lightList;
		std::vector<uint32_t> activeLights;
		std::vector<uint32_t> emptyIndices;
	}
	namespace Light
	{
		void initialize()
		{
			
		}
		
		void cleanup()
		{
			for(uint i = 0; i < activeLights.size(); i++)
				remove(activeLights[i]);

			activeLights.clear();
			lightList.clear();
			emptyIndices.clear();
		}

		std::vector<uint32_t>* getActiveLights()
		{
			return &activeLights;
		}
		
		void remove(uint32_t index)
		{
			if(index < lightList.size())
			{
				bool alreadyEmpty = true;
				for(uint32_t activeLight : activeLights)
				{
					if(activeLight == index)
					{
						alreadyEmpty = false;
						break;
					}
				}

				if(!alreadyEmpty)
				{
					emptyIndices.push_back(index);
					lightList[index].valid = false;
					activeLights.erase(activeLights.begin() + index);
				}
				else
				{
					Log::warning("Light is already removed!");
				}
			}
		}
		
		CLight* getLightAtIndex(uint32_t index)
		{
			if(index < lightList.size())
			{
				return &lightList[index];
			}
			else
			{
				Log::error("Light::getLightByIndex", "Invalid light index");
				return NULL;
			}
		}
		
		int create(Node node)
		{
			int index = -1;
			if(emptyIndices.empty())
			{
				lightList.push_back(CLight());
				index = lightList.size() - 1;
			}
			else
			{
				index = emptyIndices.back();
				lightList[index] = CLight();
			}

			lightList[index].node = node;
			activeLights.push_back(index);
			
			return index;
		}
		
		// void setRadius(CLight* light, float radius)
		// {
		// 	light->radius = radius;
		// 	h3dSetNodeParamF(light->node, H3DLight::RadiusF, 0, radius);
		// }
		
		// void setColor(CLight* light, Vec3 color)
		// {
		// 	light->color = color;
		// 	h3dSetNodeParamF(light->node, H3DLight::ColorF3, 0, color.r);
		// 	h3dSetNodeParamF(light->node, H3DLight::ColorF3, 1, color.g);
		// 	h3dSetNodeParamF(light->node, H3DLight::ColorF3, 2, color.b);
		// }
		
		// void setFov(CLight* light, float fov)
		// {
		// 	light->fov = fov;
		// 	h3dSetNodeParamF(light->node, H3DLight::FovF, 0, fov);
		// }

		// void setShadowCaster(CLight* light, bool enable)
		// {
		// 	if(enable)
		// 	{
		// 		light->castShadow = true;
		// 		setShadowMapCount(light, light->shadowmapCount);
		// 	}
		// 	else
		// 	{
		// 		light->castShadow = false;
		// 		setShadowMapCount(light, 0);
		// 	}
		// }
		
		// void setShadowMapCount(CLight* light, int count)
		// {
		// 	light->shadowmapCount = count;
			
		// 	h3dSetNodeParamI(light->node, H3DLight::ShadowMapCountI, count);

		// 	if(light->shadowmapCount > 0 && !light->castShadow)
		// 		light->castShadow = true;
		// 	else if(light->shadowmapCount == 0 && light->castShadow)
		// 		light->castShadow = false;
		// }
		
		// void setShadowMapBias(CLight* light, float bias)
		// {
		// 	light->shadowmapBias = bias;
		// 	h3dSetNodeParamF(light->node, H3DLight::ShadowMapBiasF, 0, bias);
		// }

		// void setShadowSplitLambda(CLight* light, float splitLambda)
		// {
		// 	light->shadowSplitLambda = splitLambda;
		// 	h3dSetNodeParamF(light->node,
		// 					 H3DLight::ShadowSplitLambdaF,
		// 					 0,
		// 					 splitLambda);
		// }
		
		// void setIntensity(CLight* light, float intensity)
		// {
		// 	light->intensity = intensity;
		// 	h3dSetNodeParamF(light->node, H3DLight::ColorMultiplierF, 0, intensity);
		// }

		void generateBindings()
		{
			Sqrat::RootTable().Bind("CLight", Sqrat::Class<CLight>()
								.Var("intensity",     &CLight::intensity)
								.Var("color",         &CLight::color)
								.Var("castShadow",    &CLight::castShadow));

			// Sqrat::RootTable().Bind("Light", Sqrat::Table(ScriptEngine::getVM())
			// 					.Func("setRadius", &setRadius)
			// 					.Func("setColor", &setColor)
			// 					.Func("setIntensity", &setIntensity)
			// 					.Func("setShadowBias", &setShadowMapBias)
			// 					.Func("setShadowLambda", &setShadowSplitLambda)
			// 					.Func("setShadowmapCount", &setShadowMapCount)
			// 					.Func("setShadowCaster", &setShadowCaster));
		}
	}

}
