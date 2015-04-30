#include <GL/glew.h>
#include <GL/gl.h>

#include "model.h"
#include "camera.h"
#include "scriptengine.h"
#include "shader.h"
#include "gameobject.h"
#include "scenemanager.h"
#include "transform.h"
#include "light.h"
#include "texture.h"
#include "renderer.h"
#include "passert.h"
#include "geometry.h"
#include "boundingvolumes.h"
#include "editor.h"

namespace Model
{
	namespace
	{
		std::vector<CModel>        modelList;
		std::vector<unsigned int>  emptyIndices;
		int                        culled      = 0;
		int                        rendered    = 0;
		int                        lightCount  = 0;
		int                        totalVertCount   = 0;
  	}

	int setLights(int shaderIndex, CCamera* camera)
	{
		std::vector<uint32_t>* activeLights = Light::getActiveLights();
		uint32_t count = 0;
		for(uint32_t lightIndex : *activeLights)
		{
			CLight*     light      = Light::getLightAtIndex(lightIndex);
			GameObject* gameObject = SceneManager::find(light->node);
			CTransform* transform  = GO::getTransform(gameObject);

			if(light->type != LT_DIR &&
			   !BoundingVolume::isIntersecting(&camera->frustum, &light->boundingSphere, transform))
				continue;
			
			std::string arrayIndex = "lightList[" + std::to_string(count) + "].";
			Shader::setUniformFloat(shaderIndex,
									std::string(arrayIndex + "intensity").c_str(),
									light->intensity);
			Shader::setUniformFloat(shaderIndex,
									std::string(arrayIndex + "outerAngle").c_str(),
									light->outerAngle);
			Shader::setUniformFloat(shaderIndex,
									std::string(arrayIndex + "innerAngle").c_str(),
									light->innerAngle);
			Shader::setUniformFloat(shaderIndex,
									std::string(arrayIndex + "falloff").c_str(),
									light->falloff);
			Shader::setUniformInt(shaderIndex,
								  std::string(arrayIndex + "radius").c_str(),
								  light->radius);
			Shader::setUniformInt(shaderIndex,
								  std::string(arrayIndex + "type").c_str(),
								  light->type);
			Shader::setUniformVec4(shaderIndex,
								   std::string(arrayIndex + "color").c_str(),
								   light->color);
			Shader::setUniformVec3(shaderIndex,
								   std::string(arrayIndex + "direction").c_str(),
								   transform->forward);
			Shader::setUniformVec3(shaderIndex,
								   std::string(arrayIndex + "position").c_str(),
								   transform->position);
			count++;
			if(count > (Light::MAX_LIGHTS - 1))
			{
				count--;
				break;
			}
		}
		Shader::setUniformInt(shaderIndex, "numLights", count);
		return count;
	}
		
	void renderAllModels(CCamera* camera, RenderParams* renderParams)
	{
		GameObject* viewer = SceneManager::find(camera->node);
		CTransform* viewerTransform = GO::getTransform(viewer);
		
		for(Mat_Type material : Material::MATERIAL_LIST)
		{
			// TODO: Add error checking for returned material params
			std::vector<int>* registeredMeshes = Material::getRegisteredModels(material);
			int shaderIndex = Material::getShaderIndex(material);
			Shader::bind(shaderIndex);
				
			if((material == MAT_PHONG || material == MAT_PHONG_TEXTURED)
			   && registeredMeshes->size() > 0)
			{
				lightCount = setLights(shaderIndex, camera);					
			}
			// Setup uniforms for material
			Shader::setUniformVec3(shaderIndex, "eyePos", viewerTransform->position);
			Shader::setUniformFloat(shaderIndex, "fog.density",  renderParams->fog.density);
			Shader::setUniformFloat(shaderIndex, "fog.start",    renderParams->fog.start);
			Shader::setUniformFloat(shaderIndex, "fog.max",      renderParams->fog.max);
			Shader::setUniformInt(shaderIndex,   "fog.fogMode",  renderParams->fog.fogMode);
			Shader::setUniformVec4(shaderIndex,  "fog.color",    renderParams->fog.color);
			if(material == MAT_PHONG || material == MAT_PHONG_TEXTURED)
				Shader::setUniformVec4(shaderIndex,  "ambientLight", renderParams->ambientLight);
				
			for(int modelIndex : *registeredMeshes)
			{
				const CModel* model      = &modelList[modelIndex];
				GameObject*   gameObject = SceneManager::find(model->node);
				CTransform*   transform  = GO::getTransform(gameObject);					
				Mat4          mvp        = camera->viewProjMat * transform->transMat;

				Shader::setUniformMat4(shaderIndex, "mvp", mvp);
				Shader::setUniformMat4(shaderIndex, "modelMat", transform->transMat);
				Material::setMaterialUniforms(&model->materialUniforms, (Mat_Type)model->material);
				int vertCount = Geometry::render(model->geometryIndex, &camera->frustum, transform);
				totalVertCount += vertCount;
				if(vertCount > 0)
					rendered++;
				else
					culled++;
				if(model->material == MAT_UNSHADED_TEXTURED || model->material == MAT_PHONG_TEXTURED)
					Texture::unbind();
			}
			Shader::unbind();
		}
		Editor::addDebugInt("Vertices", totalVertCount);
		Editor::addDebugInt("Tris", totalVertCount / 3);
		Editor::addDebugInt("Rendered", rendered);
		Editor::addDebugInt("Culled", culled);
		Editor::addDebugInt("ActiveLights", lightCount);
		Editor::addDebugInt("Total Lights", Light::getActiveLights()->size());
		rendered       = 0;
		culled         = 0;
		lightCount     = 0;
		totalVertCount = 0;
	}

	int create(const char* filename)
	{
		int index = -1;
		CModel model;
		int geometryIndex = Geometry::create(filename);
		if(geometryIndex != -1)
		{
			if(emptyIndices.empty())
			{
				modelList.push_back(model);
				index = modelList.size() - 1;
			}
			else
			{
				index = emptyIndices.back();
				modelList[index] = model;
				emptyIndices.pop_back();
			}
			if(!Material::registerModel(index, (Mat_Type)model.material))
				Log::warning("Model already registered with material");
			modelList[index].materialUniforms.texture = Texture::create("default.png");
			modelList[index].geometryIndex = geometryIndex;
		}
		else
		{
			Log::error("Model::create", std::string(filename) + " not found!");
		}
		return index;
	}

	CModel* getModelAtIndex(int modelIndex)
	{
		CModel* model = NULL;
		if(modelIndex > -1 && modelIndex < (int)modelList.size())
			model = &modelList[modelIndex];
		else
			Log::error("Model::getModelAtindex", "Invalid modelIndex");
		return model;
	}


	void cleanup()
	{
		for(unsigned int i = 0; i < modelList.size(); i++)
			remove(i);

		modelList.clear();
		emptyIndices.clear();
	}

    bool writeToJSON(CModel* model, rapidjson::Writer<rapidjson::StringBuffer>& writer)
	{
		using namespace rapidjson;
		bool success = true;
		writer.Key("Model");
		writer.StartObject();

		writer.Key("Geometry"); writer.String(Geometry::getName(model->geometryIndex).c_str());
		writer.Key("Material"); writer.Int(model->material);

		writer.Key("MaterialUniforms");
        writer.StartObject();
		writer.Key("Diffuse");          writer.Double(model->materialUniforms.diffuse);
		writer.Key("Specular");         writer.Double(model->materialUniforms.specular);
		writer.Key("SpecularStrength"); writer.Double(model->materialUniforms.specularStrength);
		writer.Key("DiffuseColor");
		writer.StartArray();
		for(int i = 0; i < 4; i++)      writer.Double(model->materialUniforms.diffuseColor[i]);
		writer.EndArray();
        writer.EndObject();
		
		writer.EndObject();
		return success;
	}
		
	void generateBindings()
	{
		asIScriptEngine* engine = ScriptEngine::getEngine();
		int rc = engine->RegisterObjectType("Model", sizeof(CModel), asOBJ_REF | asOBJ_NOCOUNT);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Model", "int32 node", asOFFSET(CModel, node));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Model", "int32 material", asOFFSET(CModel, material));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Model",
											"Mat_Uniforms materialUniforms",
											asOFFSET(CModel, materialUniforms));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Model",
										  "bool setMaterialType(Mat_Type)",
										  asFUNCTION(setMaterialType),
										  asCALL_CDECL_OBJFIRST);
		rc = engine->RegisterObjectMethod("Model",
										  "bool setGeometry(const string &in)",
										  asFUNCTION(setGeometry),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
	}

	void remove(int modelIndex)
	{
		CModel* model = &modelList[modelIndex];
		model->node   = -1;
		Material::removeMaterialUniforms(&model->materialUniforms, (Mat_Type)model->material);
		if(!Material::unRegisterModel(modelIndex, (Mat_Type)model->material))
			Log::warning("Model at index " + std::to_string(modelIndex) + " not unregistered");
		Geometry::remove(model->geometryIndex);
		emptyIndices.push_back(modelIndex);
	}
		
	void initialize()
	{
	}

	bool setMaterialType(CModel* model, Mat_Type material)
	{
		int index = -1;
		bool success = true;
		for(int i = 0; i < (int)modelList.size(); i++)
		{
			if(model->node == modelList[i].node)
			{
				index = i;
				break;
			}
		}
		if(index != -1)
		{
			if(Material::unRegisterModel(index, (Mat_Type)model->material))
			{
				model->material = material;
				if(!Material::registerModel(index, material))
				{
					Log::error("Model::setMaterialType", "Model could not be registered");
					success = false;
				}
			}
			else
			{
				Log::error("Model::setMaterialType", "Model could not be unregistered");
				success = false;
			}
		}
		else
		{
			Log::error("Model::setMaterialtype", "Model not found");
			success = false;
		}
		return success;
	}

	bool createFromJSON(CModel* model, const rapidjson::Value& value)
	{
		using namespace rapidjson;
		bool success = true;
		const char* error = "Invalid value in a field";
		PA_ASSERT(model);

		if(value.IsObject())
		{
			if(value.HasMember("Geometry") && value["Geometry"].IsString())
			{
				const Value& geometryNode = value["Geometry"];
				const std::string& filename = geometryNode.GetString();
				setGeometry(model, filename.c_str());
			}
			else
			{
				success = false;
				Log::error("Model::createFromJSON", "Error loading Geometry");
			}

			if(value.HasMember("Material") && value["Material"].IsInt())
			{
				const Value& materialNode = value["Material"];
				int material = materialNode.GetInt();
				if(material > -1 && material < 4)
					setMaterialType(model, (Mat_Type)material);
				else
					success = false;
			}
			else
			{
				success = false;
				Log::error("Model::createFromJSON", "Error loading Material");
			}

			if(value.HasMember("MaterialUniforms") && value["MaterialUniforms"].IsObject())
			{
				const Value& matUniforms = value["MaterialUniforms"];
				if(matUniforms.HasMember("DiffuseColor") && matUniforms["DiffuseColor"].IsArray())
				{
					const Value& diffuseColorNode = matUniforms["DiffuseColor"];
					int items = diffuseColorNode.Size() < 4 ? diffuseColorNode.Size() : 4;
					for(int i = 0; i < items; i++)
					{
						if(diffuseColorNode[i].IsNumber())
							model->materialUniforms.diffuseColor[i] = (float)diffuseColorNode[i].GetDouble();
						else
							success = false;
					}
				}
				else
				{
					success = false;
					Log::error("Model::createFromJSON", "Error loading MaterialUniforms.DiffuseColor");
				}

				if(matUniforms.HasMember("Texture") && matUniforms["Texture"].IsString())
				{
					const Value& textureNode = matUniforms["Texture"];
					const std::string& filename = textureNode.GetString();
					int index = Texture::create(filename.c_str());
					if(index > -1) model->materialUniforms.texture = index;
				}

				if(matUniforms.HasMember("Diffuse") && matUniforms["Diffuse"].IsNumber())
				{
					const Value& diffuseNode = matUniforms["Diffuse"];
					float diffuse = (float)diffuseNode.GetDouble();
					if(diffuse >= 0)
						model->materialUniforms.diffuse = glm::clamp(diffuse, 0.f, 1.f);
					else
						success = false;
				}
				else
				{
					success = false;
					Log::error("Model::createFromJSON", "Error loading Diffuse");
				}

				if(matUniforms.HasMember("Specular") && matUniforms["Specular"].IsNumber())
				{
					const Value& specularNode = matUniforms["Specular"];
					float specular = (float)specularNode.GetDouble();
					if(specular >= 0)
						model->materialUniforms.specular = glm::clamp(specular, 0.f, 1.f);
					else
						success = false;
				}
				else
				{
					success = false;
					Log::error("Model::createFromJSON", "Error loading Specular");
				}

				if(matUniforms.HasMember("SpecularStrength") && matUniforms["SpecularStrength"].IsNumber())
				{
					const Value& specularStrengthNode = matUniforms["SpecularStrength"];
					float specularStrength = (float)specularStrengthNode.GetDouble();
					if(specularStrength >= 0)
						model->materialUniforms.specularStrength = specularStrength;
					else
						success = false;
				}
				else
				{
					success = false;
					Log::error("Model::createFromJSON", "Error loading SpecularStrength");
				}
			}
			else
			{
				success = false;
				Log::error("Model::createFromJSON", "Error loading MaterialUniforms");
			}
		}
		else
		{
			error   = "'Model' must be an object";
			success = false;
		}
		if(!success) Log::error("Model::createFromJSON", error);
		return success;
	}

	bool setGeometry(CModel* model, const std::string& filename)
	{
		PA_ASSERT(model);
		bool success = true;
		int geometryIndex = Geometry::create(filename.c_str());
		if(geometryIndex > -1)
			model->geometryIndex = geometryIndex;
		else
			success = false;
		return success; 
	}
}

