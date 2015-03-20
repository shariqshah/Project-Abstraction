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
				uint indexToErase = 0;
				for(uint i = 0; i < activeLights.size(); i++)
				{
					if(activeLights[i] == index)
					{
						alreadyEmpty = false;
						indexToErase = i;
						break;
					}
				}

				if(!alreadyEmpty)
				{
					emptyIndices.push_back(index);
					lightList[index].valid = false;
					activeLights.erase(activeLights.begin() + indexToErase);
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

		void generateBindings()
		{
			Sqrat::ConstTable().Enum("LightType", Sqrat::Enumeration ()
									 .Const("SPOT",  LightType::LT_SPOT)
									 .Const("DIR",   LightType::LT_DIR)
									 .Const("POINT", LightType::LT_POINT));
			
			Sqrat::RootTable().Bind("CLight", Sqrat::Class<CLight>()
								.Var("intensity",   &CLight::intensity)
								.Var("color",       &CLight::color)
								.Var("outerAngle",  &CLight::outerAngle)
								.Var("innderAngle", &CLight::innerAngle)
								.Var("exponent",    &CLight::exponent)
								.Var("node",        &CLight::node)
								.Var("castShadow",  &CLight::castShadow)
								.Var("valid",       &CLight::valid)
								.Var("lightType",   &CLight::type));

			Sqrat::RootTable().Bind("Light", Sqrat::Table(ScriptEngine::getVM())
									.Func("create", &create));
		}
	}

}
