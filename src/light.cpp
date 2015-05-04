#include <GL/gl.h>

#include "light.h"
#include "scriptengine.h"
#include "passert.h"
#include "camera.h"
#include "scenemanager.h"
#include "texture.h"
#include "gameobject.h"

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
			emptyIndices.pop_back();
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
										  "void setRadius(float)",
										  asFUNCTION(setRadius),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Light",
										  "void setType(int)",
										  asFUNCTION(setType),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Light",
										  "void setOuterAngle(float)",
										  asFUNCTION(setOuterAngle),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Light",
										  "void setInnerAngle(float)",
										  asFUNCTION(setInnerAngle),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Light",
										  "void setFalloff(float)",
										  asFUNCTION(setFalloff),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Light",
										  "void setIntensity(float)",
										  asFUNCTION(setIntensity),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Light",
										  "void setCastShadow(bool)",
										  asFUNCTION(setCastShadow),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
	}

	bool writeToJSON(CLight* light, rapidjson::Writer<rapidjson::StringBuffer>& writer)
	{
		using namespace rapidjson;
		bool success = true;
		writer.Key("Light");
		writer.StartObject();
		writer.Key("Type");        writer.Int(light->type);
		writer.Key("InnerAngle");  writer.Double(light->innerAngle);
		writer.Key("OuterAngle");  writer.Double(light->outerAngle);
		writer.Key("Falloff");     writer.Double(light->falloff);
		writer.Key("Radius");      writer.Double(light->radius);
		writer.Key("Intensity");   writer.Double(light->intensity);
		writer.Key("CastShadow");  writer.Bool(light->castShadow);
		writer.Key("PcfEnabled");  writer.Bool(light->pcfEnabled);
		writer.Key("DepthBias");   writer.Double(light->depthBias);
		writer.Key("Color");
		writer.StartArray();
		for(int i = 0; i < 4; i++) writer.Double(light->color[i]);
		writer.EndArray();
		writer.EndObject();
		return success;
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
				Log::error("Light::creatFromJSON", "Error reading Color");
			}

			if(value.HasMember("Type") && value["Type"].IsInt())
			{
				const Value& typeNode = value["Type"];
				int type = typeNode.GetInt();
				if(type >= 0 && type <= 2)
					setType(light, (LightType)type);
				else
					success = false;
			}
			else
			{
				success = false;
				Log::error("Light::creatFromJSON", "Error reading Typer");
			}

			if(value.HasMember("InnerAngle") && value["InnerAngle"].IsNumber())
			{
				const Value& innerAngleNode = value["InnerAngle"];
				float innerAngle = (float)innerAngleNode.GetDouble();
				if(innerAngle >= 0)
					light->innerAngle = innerAngle;
				else
					success = false;
			}
			else
			{
				success = false;
				Log::error("Light::creatFromJSON", "Error reading InnerAngle");
			}

			if(value.HasMember("OuterAngle") && value["OuterAngle"].IsNumber())
			{
				const Value& outerangleNode = value["OuterAngle"];
				float outerangle = (float)outerangleNode.GetDouble();
				if(outerangle >= 0)
					setOuterAngle(light, outerangle);
				else
					success = false;
			}
			else
			{
				success = false;
				Log::error("Light::creatFromJSON", "Error reading OuterAngle");
			}

			if(value.HasMember("DepthBias") && value["DepthBias"].IsNumber())
			{
				const Value& depthBiasNode = value["DepthBias"];
				float depthBias = (float)depthBiasNode.GetDouble();
				if(depthBias >= 0)
					light->depthBias = depthBias;
				else
					success = false;
			}
			else
			{
				success = false;
				Log::error("Light::creatFromJSON", "Error reading DepthBias");
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
				Log::error("Light::creatFromJSON", "Error reading Falloff");
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
				Log::error("Light::creatFromJSON", "Error reading Radius");
			}

			if(value.HasMember("Intensity") && value["Intensity"].IsNumber())
			{
				const Value& intensityNode = value["Intensity"];
				float intensity = (float)intensityNode.GetDouble();
				light->intensity = glm::clamp(intensity, 0.f, 10.f);
			}
			else
			{
				success = false;
				Log::error("Light::creatFromJSON", "Error reading Intensity");
			}

			if(value.HasMember("CastShadow") && value["CastShadow"].IsBool())
			{
				const Value& castShadowNode = value["CastShadow"];
				light->castShadow = castShadowNode.GetBool();
				setCastShadow(light, light->castShadow);
			}
			else
			{
				success = false;
				Log::error("Light::creatFromJSON", "Error reading CastShadow");
			}

			if(value.HasMember("PcfEnabled") && value["PcfEnabled"].IsBool())
			{
				const Value& pcfEnabledNode = value["PcfEnabled"];
				light->pcfEnabled = pcfEnabledNode.GetBool();
			}
			else
			{
				success = false;
				Log::error("Light::creatFromJSON", "Error reading PcfEnabled");
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
		PA_ASSERT(light);
		light->radius = radius;
		light->boundingSphere.radius = radius;
		if(light->castShadow)
		{
			CCamera* camera = Camera::getCameraAtIndex(light->node);
			camera->farZ = light->radius;
			Camera::updateProjection(camera);
		}
	}

	void setType(CLight* light, LightType type)
	{
		PA_ASSERT(light);
		light->type = type;
		if(light->castShadow)
		{
			CCamera* camera = Camera::getCameraAtIndex(light->node);
			Camera::setOrthographic(camera, type == LT_DIR ? true : false);
		}
	}
	
	void setOuterAngle(CLight* light, float outerAngle)
	{
		PA_ASSERT(light);
		light->outerAngle = outerAngle;
		if(light->castShadow)
		{
			CCamera* camera = Camera::getCameraAtIndex(light->node);
			camera->fov = outerAngle * 1.5;
			Camera::updateProjection(camera);
		}
	}

	void setInnerAngle(CLight* light, float innerAngle)
	{
		PA_ASSERT(light);
		light->innerAngle = innerAngle;
	}
	
	void setFalloff(CLight* light, float falloff)
	{
		PA_ASSERT(light);
		light->falloff = falloff;
	}
	
	void setIntensity(CLight* light, float intensity)
	{
		PA_ASSERT(light);
		light->intensity = glm::clamp(intensity, 0.f, 10.f);
	}
	
	void setCastShadow(CLight* light, bool castShadow)
	{
		PA_ASSERT(light);
		light->castShadow = castShadow;
		if(castShadow && light->depthMap == -1)
		{
			std::string depthMapName = "DepthMap" + std::to_string(light->node);
			int texture = Texture::create(depthMapName.c_str(),
										  Settings::getRenderWidth(),
										  Settings::getRenderHeight(),
										  GL_DEPTH_COMPONENT,
										  GL_DEPTH_COMPONENT24,
										  GL_FLOAT,
										  NULL);
			light->depthMap = texture;
			Texture::setTextureParameter(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			Texture::setTextureParameter(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			Texture::setTextureParameter(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			Texture::setTextureParameter(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			Texture::setTextureParameter(texture, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			Texture::setTextureParameter(texture, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

			GameObject* gameobject = SceneManager::find(light->node);
			CCamera* camera = GO::getCamera(gameobject);
			if(!camera)	camera = GO::addCamera(gameobject);
			camera->farZ = light->radius;
			camera->nearZ = 1.5f;
			camera->fov   = light->radius * 1.5f;
			camera->aspectRatio = Settings::getRenderWidth() / Settings::getRenderHeight();
			if(light->type == LT_DIR)
			{
				Camera::setOrthographic(camera, true);
				Camera::updateView(camera);
			}
			else
			{
				Camera::setOrthographic(camera, false);
				Camera::updateView(camera);
				Camera::updateProjection(camera);
			}
		}
		else if(!castShadow && light->depthMap != -1)
		{
			Texture::remove(light->depthMap);
			light->depthMap = -1;
		}
	}
}
