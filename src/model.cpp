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

namespace Model
{
	namespace
	{
		std::vector<CModel>        modelList;
		std::vector<unsigned int>  emptyIndices;
		char* modelPath;
			
		void createVAO(CModel* model)
		{
			// TODO : Add support for different model formats and interleaving VBO
			PA_ASSERT(model);

			glGenVertexArrays(1, &model->vao);
			glBindVertexArray(model->vao);

			GLuint vertexVBO;
			glGenBuffers(1, &vertexVBO);
			glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
			glBufferData(GL_ARRAY_BUFFER,
						 model->vertices.size() * sizeof(Vec3),
						 model->vertices.data(),
						 GL_STATIC_DRAW);
			Renderer::checkGLError("Model::createVBO::vertex");
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

			if(model->normals.size() > 0)
			{
				GLuint normalVBO;
				glGenBuffers(1, &normalVBO);
				glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
				glBufferData(GL_ARRAY_BUFFER,
							 model->normals.size() * sizeof(Vec3),
							 model->normals.data(),
							 GL_STATIC_DRAW);
				Renderer::checkGLError("Model::createVBO::normal");
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, 0);
			}

			if(model->uvs.size() > 0)
			{
				GLuint uvVBO;
				glGenBuffers(1, &uvVBO);
				glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
				glBufferData(GL_ARRAY_BUFFER,
							 model->uvs.size() * sizeof(Vec2),
							 model->uvs.data(),
							 GL_STATIC_DRAW);
				Renderer::checkGLError("Model::createVBO::uv");
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
			}

			if(model->vertexColors.size() > 0)
			{
				GLuint colorVBO;
				glGenBuffers(1, &colorVBO);
				glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
				glBufferData(GL_ARRAY_BUFFER,
							 model->vertexColors.size() * sizeof(Vec3),
							 model->vertexColors.data(),
							 GL_STATIC_DRAW);
				Renderer::checkGLError("Model::createVBO::color");
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
			}

			if(model->indices.size() > 0)
			{
				GLuint ibo;
				glGenBuffers(1, &ibo);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER,
							 model->indices.size() * sizeof(GLuint),
							 model->indices.data(),
							 GL_STATIC_DRAW);
				model->drawIndexed = true;
			}
		
			glBindVertexArray(0);
		}

		int findModelIndex(const char* name)
		{
			int loaded = -1;
			for(uint i = 0; i < modelList.size(); i++)
			{
				if(strcmp(name, modelList[i].filename.c_str()) == 0)
				{
					loaded = i;
					break;
				}
			}

			return loaded;
		}
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

				// Very Simple culling
				// int distance = glm::distance(transform->position, viewerTransform->position);
				// if(distance > 100)
				// 	continue;
					
				Mat4 mvp = camera->viewProjMat * transform->transMat;
				Shader::setUniformMat4(shaderIndex, "mvp", mvp);
				Shader::setUniformMat4(shaderIndex, "modelMat", transform->transMat);
				Material::setMaterialUniforms(&model->materialUniforms, (Mat_Type)model->material);
					
				glBindVertexArray(model->vao);
				if(model->drawIndexed)
					glDrawElements(GL_TRIANGLES, model->indices.size(), GL_UNSIGNED_INT, (void*)0);
				else
					glDrawArrays(GL_TRIANGLES, 0, model->vertices.size());
				glBindVertexArray(0);

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
		if(loadFromFile(filename, &model))
		{
			createVAO(&model); // Create VAO for the model that will be used in rendering
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
			if(Material::registerModel(index, (Mat_Type)model.material))
				Log::message("Model " + model.filename + " registered");
			else
				Log::warning("Model " + model.filename + " already registered!");
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
		free(modelPath);
		for(unsigned int i = 0; i < modelList.size(); i++)
			remove(i);

		modelList.clear();
		emptyIndices.clear();
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Proxy functions for binding
	//////////////////////////////////////////////////////////////////////////////////////////

	Vec3 getVertex(CModel* model, int index)
	{
		Vec3 vertex;
		if(index >= 0 && index < (int)model->vertices.size()) vertex = model->vertices.at(index);
		else Log::error("Model::getVertex", "Invalid index");
		return vertex;
	}
		
	Vec3 getNormal(CModel* model, int index)
	{
		Vec3 normal;
		if(index >= 0 && index < (int)model->normals.size()) normal = model->normals.at(index);
		else Log::error("Model::getNormal", "Invalid index");
		return normal;
	}

	Vec2 getUV(CModel* model, int index)
	{
		Vec2 uvCoord;
		if(index >= 0 && index < (int)model->uvs.size()) uvCoord = model->uvs.at(index);
		else Log::error("Model::getUV", "Invalid index");
		return uvCoord;
	}

	uint getIndex(CModel* model, int index)
	{
		uint meshIndex = 0;
		if(index >= 0 && index < (int)model->indices.size()) meshIndex = model->indices.at(index);
		else Log::error("Model::getIndex", "Invalid index");
		return meshIndex;
	}

	void setVertex(CModel* model, int index, Vec3 value)
	{
		if(index >= 0 && index < (int)model->vertices.size()) model->vertices[index] = value;
		else Log::error("Model::setVertex", "Invalid index");
	}
		
	void setNormal(CModel* model, int index, Vec3 value)
	{
		if(index >= 0 && index < (int)model->normals.size()) model->normals[index] = value;
		else Log::error("Model::setNormal", "Invalid index");
	}

	void setUV(CModel* model, int index, Vec2 value)
	{
		if(index >= 0 && index < (int)model->uvs.size()) model->uvs[index] = value;
		else Log::error("Model::setUV", "Invalid index");
	}

	void setIndex(CModel* model, int index, uint value)
	{
		if(index >= 0 && index < (int)model->indices.size()) model->indices[index] = value;
		else Log::error("Model::setIndex", "Invalid index");
	}

	void appendVertex(CModel* model, Vec3 vertex)
	{
		model->vertices.push_back(vertex);
	}

	void appendNormal(CModel* model, Vec3 normal)
	{
		model->normals.push_back(normal);
	}

	void appendUV(CModel* model, Vec2 uv)
	{
		model->uvs.push_back(uv);
	}

	void appendIndex(CModel* model, uint index)
	{
		model->indices.push_back(index);
	}
		
	void generateBindings()
	{
		asIScriptEngine* engine = ScriptEngine::getEngine();
		int rc = engine->RegisterObjectType("Model", sizeof(CModel), asOBJ_REF | asOBJ_NOCOUNT);
		PA_ASSERT(rc >= 0);

		rc = engine->RegisterObjectProperty("Model", "int32 node", asOFFSET(CModel, node));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Model", "string filename", asOFFSET(CModel, filename));
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
		rc = engine->RegisterObjectMethod("Model",
										  "Vec3 getVertex(int)",
										  asFUNCTION(getVertex),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Model",
										  "Vec3 getNormal(int)",
										  asFUNCTION(getNormal),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Model",
										  "Vec2 getUV(int)",
										  asFUNCTION(getUV),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Model",
										  "uint getIndex(int)",
										  asFUNCTION(getIndex),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Model",
										  "void setVertex(int, Vec3)",
										  asFUNCTION(setVertex),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Model",
										  "void setNormal(int, Vec3)",
										  asFUNCTION(setNormal),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Model",
										  "void setUV(int, Vec2)",
										  asFUNCTION(setUV),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Model",
										  "void setIndex(int, uint)",
										  asFUNCTION(setIndex),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Model",
										  "void appendVertex(Vec3)",
										  asFUNCTION(appendVertex),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Model",
										  "void appendNormal(Vec3)",
										  asFUNCTION(appendNormal),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Model",
										  "void appendUV(Vec2)",
										  asFUNCTION(appendUV),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectMethod("Model",
										  "void appendIndex(uint)",
										  asFUNCTION(appendIndex),
										  asCALL_CDECL_OBJFIRST);
		PA_ASSERT(rc >= 0);
	}

	void remove(unsigned int modelIndex)
	{
		CModel* model = &modelList[modelIndex];
		glDeleteVertexArrays(1, &model->vao);

		Material::removeMaterialUniforms(&model->materialUniforms, (Mat_Type)model->material);
		model->vertices.clear();
		model->normals.clear();
		model->uvs.clear();
		model->vertexColors.clear();

		if(!Material::unRegisterModel(modelIndex, (Mat_Type)model->material))
			Log::warning("Model at index " + std::to_string(modelIndex) + " not unregistered");
		emptyIndices.push_back(modelIndex);
	}
		
	void initialize(const char* path)
	{
		modelPath   = (char *)malloc(sizeof(char) * strlen(path) + 1);
		strcpy(modelPath, path);
	}

	bool loadFromFile(const char* filename, CModel* model)
	{
		PA_ASSERT(model);
		PA_ASSERT(filename);
		bool success = true;
		int index = findModelIndex(filename);
		if(index == -1)
		{
			char* fullPath = (char *)malloc(sizeof(char) *
											(strlen(modelPath) + strlen(filename)) + 1);
			strcpy(fullPath, modelPath);
			strcat(fullPath, filename);
			
			FILE* file = fopen(fullPath, "rb");
			free(fullPath);
			if(file)
			{					
				const uint32_t INDEX_SIZE = sizeof(uint32_t);
				const uint32_t VEC3_SIZE  = sizeof(Vec3);
				const uint32_t VEC2_SIZE  = sizeof(Vec2);
				uint32_t header[4];
				size_t bytesRead = 0;
				if((bytesRead = fread(header, INDEX_SIZE, 4, file)) <= 0)
				{
					Log::error("Model::loadFromFile", "Read failed");
					success = false;
				}
				else
				{
					uint32_t indicesCount  = header[0];
					uint32_t verticesCount = header[1];
					uint32_t normalsCount  = header[2];
					uint32_t uvsCount      = header[3];
					// Indices
					model->indices.reserve(indicesCount);
					model->indices.insert(model->indices.begin(), indicesCount, 0);				
					fread(&model->indices[0], INDEX_SIZE, indicesCount, file);
					// Vertices
					model->vertices.reserve(verticesCount);
					model->vertices.insert(model->vertices.begin(), verticesCount, Vec3(0.f));
					fread(&model->vertices[0], VEC3_SIZE, verticesCount, file);
					// Normals
					model->normals.reserve(normalsCount);
					model->normals.insert(model->normals.begin(), normalsCount, Vec3(0.f));				
					fread(&model->normals[0], VEC3_SIZE, normalsCount, file);
					// UVs
					model->uvs.reserve(uvsCount);
					model->uvs.insert(model->uvs.begin(), uvsCount, Vec2(0.f));				
					fread(&model->uvs[0], VEC2_SIZE, uvsCount, file);					
				}
				fclose(file);
				model->filename = filename;
				model->drawIndexed = true;
			}
			else
			{
				success = false;
			}
		}
		else
		{
			CModel existingModel = modelList[index];
			model->vertices = existingModel.vertices;
			model->indices  = existingModel.indices;
			model->normals  = existingModel.normals;
			model->uvs      = existingModel.uvs;
			model->filename = existingModel.filename;
		}
		return success;
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

	CModel* findModel(const char* filename)
	{
		PA_ASSERT(filename);
		CModel* model = NULL;
		int index = findModelIndex(filename);
		if(index) model = &modelList[index];
		return model;
	}
}

