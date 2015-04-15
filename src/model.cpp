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

namespace Model
{
	namespace
	{
		std::vector<CModel>        modelList;
		std::vector<unsigned int>  emptyIndices;
  	}

	void setLights(int shaderIndex)
	{
		// TODO: Add light culling!
		std::vector<uint32_t>* activeLights = Light::getActiveLights();
		uint32_t count = 0;
		for(uint32_t lightIndex : *activeLights)
		{
			CLight* light = Light::getLightAtIndex(lightIndex);
				
			std::string arrayIndex = "lightList[" + std::to_string(count) + "].";
			Shader::setUniformFloat(shaderIndex,
									std::string(arrayIndex + "exponent").c_str(),
									light->exponent);
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

			GameObject* gameObject = SceneManager::find(light->node);
			CTransform* transform  = GO::getTransform(gameObject);
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
			Shader::bindShader(shaderIndex);
				
			if((material == MAT_PHONG || material == MAT_PHONG_TEXTURED)
			   && registeredMeshes->size() > 0)
			{
				setLights(shaderIndex);					
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
				Mat4 mvp                 = camera->viewProjMat * transform->transMat;
				Shader::setUniformMat4(shaderIndex, "mvp", mvp);
				Shader::setUniformMat4(shaderIndex, "modelMat", transform->transMat);
				Material::setMaterialUniforms(&model->materialUniforms, (Mat_Type)model->material);
				Geometry::render(model->geometryIndex); // The actual render call
				if(model->material == MAT_UNSHADED_TEXTURED || model->material == MAT_PHONG_TEXTURED)
					Texture::unbindActiveTexture();
			}
			Shader::unbindActiveShader();
		}
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
			Material::registerModel(index, (Mat_Type)model.material);
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

	//////////////////////////////////////////////////////////////////////////////////////////
	// Proxy functions for binding
	//////////////////////////////////////////////////////////////////////////////////////////

	// Vec3 getVertex(CModel* model, int index)
	// {
	// 	Vec3 vertex;
	// 	if(index >= 0 && index < (int)model->vertices.size()) vertex = model->vertices.at(index);
	// 	else Log::error("Model::getVertex", "Invalid index");
	// 	return vertex;
	// }
		
	// Vec3 getNormal(CModel* model, int index)
	// {
	// 	Vec3 normal;
	// 	if(index >= 0 && index < (int)model->normals.size()) normal = model->normals.at(index);
	// 	else Log::error("Model::getNormal", "Invalid index");
	// 	return normal;
	// }

	// Vec2 getUV(CModel* model, int index)
	// {
	// 	Vec2 uvCoord;
	// 	if(index >= 0 && index < (int)model->uvs.size()) uvCoord = model->uvs.at(index);
	// 	else Log::error("Model::getUV", "Invalid index");
	// 	return uvCoord;
	// }

	// uint getIndex(CModel* model, int index)
	// {
	// 	uint meshIndex = 0;
	// 	if(index >= 0 && index < (int)model->indices.size()) meshIndex = model->indices.at(index);
	// 	else Log::error("Model::getIndex", "Invalid index");
	// 	return meshIndex;
	// }

	// void setVertex(CModel* model, int index, Vec3 value)
	// {
	// 	if(index >= 0 && index < (int)model->vertices.size()) model->vertices[index] = value;
	// 	else Log::error("Model::setVertex", "Invalid index");
	// }
		
	// void setNormal(CModel* model, int index, Vec3 value)
	// {
	// 	if(index >= 0 && index < (int)model->normals.size()) model->normals[index] = value;
	// 	else Log::error("Model::setNormal", "Invalid index");
	// }

	// void setUV(CModel* model, int index, Vec2 value)
	// {
	// 	if(index >= 0 && index < (int)model->uvs.size()) model->uvs[index] = value;
	// 	else Log::error("Model::setUV", "Invalid index");
	// }

	// void setIndex(CModel* model, int index, uint value)
	// {
	// 	if(index >= 0 && index < (int)model->indices.size()) model->indices[index] = value;
	// 	else Log::error("Model::setIndex", "Invalid index");
	// }

	// void appendVertex(CModel* model, Vec3 vertex)
	// {
	// 	model->vertices.push_back(vertex);
	// }

	// void appendNormal(CModel* model, Vec3 normal)
	// {
	// 	model->normals.push_back(normal);
	// }

	// void appendUV(CModel* model, Vec2 uv)
	// {
	// 	model->uvs.push_back(uv);
	// }

	// void appendIndex(CModel* model, uint index)
	// {
	// 	model->indices.push_back(index);
	// }
		
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
										  "void setMaterialType(Mat_Type)",
										  asFUNCTION(setMaterialType),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		// rc = engine->RegisterObjectMethod("Model",
		// 								  "Vec3 getVertex(int)",
		// 								  asFUNCTION(getVertex),
		// 								  asCALL_CDECL_OBJFIRST);
		// PA_ASSERT(rc >= 0);
		// rc = engine->RegisterObjectMethod("Model",
		// 								  "Vec3 getNormal(int)",
		// 								  asFUNCTION(getNormal),
		// 								  asCALL_CDECL_OBJFIRST);
		// PA_ASSERT(rc >= 0);
		// rc = engine->RegisterObjectMethod("Model",
		// 								  "Vec2 getUV(int)",
		// 								  asFUNCTION(getUV),
		// 								  asCALL_CDECL_OBJFIRST);
		// PA_ASSERT(rc >= 0);
		// rc = engine->RegisterObjectMethod("Model",
		// 								  "uint getIndex(int)",
		// 								  asFUNCTION(getIndex),
		// 								  asCALL_CDECL_OBJFIRST);
		// PA_ASSERT(rc >= 0);
		// rc = engine->RegisterObjectMethod("Model",
		// 								  "void setVertex(int, Vec3)",
		// 								  asFUNCTION(setVertex),
		// 								  asCALL_CDECL_OBJFIRST);
		// PA_ASSERT(rc >= 0);
		// rc = engine->RegisterObjectMethod("Model",
		// 								  "void setNormal(int, Vec3)",
		// 								  asFUNCTION(setNormal),
		// 								  asCALL_CDECL_OBJFIRST);
		// PA_ASSERT(rc >= 0);
		// rc = engine->RegisterObjectMethod("Model",
		// 								  "void setUV(int, Vec2)",
		// 								  asFUNCTION(setUV),
		// 								  asCALL_CDECL_OBJFIRST);
		// PA_ASSERT(rc >= 0);
		// rc = engine->RegisterObjectMethod("Model",
		// 								  "void setIndex(int, uint)",
		// 								  asFUNCTION(setIndex),
		// 								  asCALL_CDECL_OBJFIRST);
		// PA_ASSERT(rc >= 0);
		// rc = engine->RegisterObjectMethod("Model",
		// 								  "void appendVertex(Vec3)",
		// 								  asFUNCTION(appendVertex),
		// 								  asCALL_CDECL_OBJFIRST);
		// PA_ASSERT(rc >= 0);
		// rc = engine->RegisterObjectMethod("Model",
		// 								  "void appendNormal(Vec3)",
		// 								  asFUNCTION(appendNormal),
		// 								  asCALL_CDECL_OBJFIRST);
		// PA_ASSERT(rc >= 0);
		// rc = engine->RegisterObjectMethod("Model",
		// 								  "void appendUV(Vec2)",
		// 								  asFUNCTION(appendUV),
		// 								  asCALL_CDECL_OBJFIRST);
		// PA_ASSERT(rc >= 0);
		// rc = engine->RegisterObjectMethod("Model",
		// 								  "void appendIndex(uint)",
		// 								  asFUNCTION(appendIndex),
		// 								  asCALL_CDECL_OBJFIRST);
		// PA_ASSERT(rc >= 0);
	}

	void remove(unsigned int modelIndex)
	{
		CModel* model = &modelList[modelIndex];
		Material::removeMaterialUniforms(&model->materialUniforms, (Mat_Type)model->material);
		if(!Material::unRegisterModel(modelIndex, (Mat_Type)model->material))
			Log::warning("Model at index " + std::to_string(modelIndex) + " not unregistered");
		Geometry::remove(model->geometryIndex);
		emptyIndices.push_back(modelIndex);
	}
		
	void initialize()
	{
	}

	void setMaterialType(CModel* model, Mat_Type material)
	{
		int index = -1;
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
			Material::unRegisterModel(index, (Mat_Type)model->material);
			model->material = material;
			Material::registerModel(index, material);
		}
		else
		{
			Log::error("Model::setMaterialtype", "Model not found");
		}
	}
}

