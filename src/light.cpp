#include "light.h"
#include "scriptengine.h"
#include "passert.h"

namespace Light
{
	namespace
	{
		std::vector<CLight>   lightList;
		std::vector<uint32_t> activeLights;
		std::vector<uint32_t> emptyIndices;
	}
	
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
		setRadius(&lightList[index], lightList[index].radius);
		return index;
	}

	void generateBindings()
	{
		asIScriptEngine* engine = ScriptEngine::getEngine();
		int rc = engine->RegisterEnum("LightType"); PA_ASSERT(rc >= 0);
		rc = engine->RegisterEnumValue("LightType", "SPOT", (int)LightType::LT_SPOT);
		rc = engine->RegisterEnumValue("LightType", "DIR", (int)LightType::LT_DIR);
		rc = engine->RegisterEnumValue("LightType", "POINT", (int)LightType::LT_POINT);
			
		rc = engine->RegisterObjectType("Light", sizeof(CLight), asOBJ_REF | asOBJ_NOCOUNT);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Light", "float intensity", asOFFSET(CLight, intensity));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Light", "Vec4 color", asOFFSET(CLight, color));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Light", "float outerAngle", asOFFSET(CLight, outerAngle));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Light", "float innerAngle", asOFFSET(CLight, innerAngle));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Light", "int32 node", asOFFSET(CLight, node));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Light", "bool castShadow", asOFFSET(CLight, castShadow));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Light", "float falloff", asOFFSET(CLight, falloff));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Light", "float radius", asOFFSET(CLight, radius));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Light", "LightType type", asOFFSET(CLight, type));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Light",
										  "void setRadius(float radius)",
										  asFUNCTION(setRadius),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
	}

	bool createFromJSON(CLight* light, const rapidjson::Value& value)
	{
		using namespace rapidjson;
		bool success = true;
		const char* error = "Invalid value in a field";
		PA_ASSERT(light);

		if(value.IsObject())
		{
			if(value.HasMember("Color") && value["Color"].IsArray())
			{
				const Value& colorNode = value["Color"];
				int items = colorNode.Size() < 4 ? colorNode.Size() : 4;
				for(int i = 0; i < items; i++)
				{
					if(colorNode[i].IsNumber())
						light->color[i] = (float)colorNode[i].GetDouble();
					else
						success = false;
				}
			}
			else
			{
				success = false;
			}

			if(value.HasMember("Type") && value["Type"].IsInt())
			{
				const Value& typeNode = value["Type"];
				int type = typeNode.GetInt();
				if(type >= 0 && type <= 2)
					light->type = type;
				else
					success = false;
			}
			else
			{
				success = false;
			}

			if(value.HasMember("InnerAngle") && value["InnerAngle"].IsNumber())
			{
				const Value& innerAngleNode = value["InnerAngle"];
				float innerAngle = (float)innerAngleNode.GetDouble();
				if(innerAngle >= 0)
					light->innerAngle = glm::radians(innerAngle);
				else
					success = false;
			}
			else
			{
				success = false;
			}

			if(value.HasMember("OuterAngle") && value["OuterAngle"].IsNumber())
			{
				const Value& outerangleNode = value["OuterAngle"];
				float outerangle = (float)outerangleNode.GetDouble();
				if(outerangle >= 0)
					light->outerAngle = glm::radians(outerangle);
				else
					success = false;
			}
			else
			{
				success = false;
			}

			if(value.HasMember("Falloff") && value["Falloff"].IsNumber())
			{
				const Value& falloffNode = value["Falloff"];
				float falloff = (float)falloffNode.GetDouble();
				if(falloff >= 0)
					light->falloff = falloff;
				else
					success = false;
			}
			else
			{
				success = false;
			}

			if(value.HasMember("Radius") && value["Radius"].IsNumber())
			{
				const Value& radiusNode = value["Radius"];
				float radius = (float)radiusNode.GetDouble();
				if(radius >= 0)
					setRadius(light, radius);
				else
					success = false;
			}
			else
			{
				success = false;
			}

			if(value.HasMember("Intensity") && value["Intensity"].IsNumber())
			{
				const Value& intensityNode = value["Intensity"];
				float intensity = (float)intensityNode.GetDouble();
				light->intensity = glm::clamp(intensity, 0.f, 1.f);
			}
			else
			{
				success = false;
			}

			if(value.HasMember("CastShadow") && value["CastShadow"].IsBool())
			{
				const Value& castShadowNode = value["CastShadow"];
				light->castShadow = castShadowNode.GetBool();
			}
			else
			{
				success = false;
			}
		}
		else
		{
			error   = "'Light' must be an object";
			success = false;
		}
		if(!success) Log::error("Light::createFromJSON", error);
		return success;
	}

	void setRadius(CLight* light, float radius)
	{
		light->radius = radius;
		light->boundingSphere.radius = radius;
	}
}
