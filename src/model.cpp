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

namespace Renderer
{
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
				assert(model);

				glGenVertexArrays(1, &model->vao);
				glBindVertexArray(model->vao);

				GLuint vertexVBO;
				glGenBuffers(1, &vertexVBO);
				glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
				glBufferData(GL_ARRAY_BUFFER,
							 model->vertices.size() * sizeof(Vec3),
							 model->vertices.data(),
							 GL_STATIC_DRAW);
				checkGLError("Model::createVBO::vertex");
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
					checkGLError("Model::createVBO::normal");
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
					checkGLError("Model::createVBO::uv");
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
					checkGLError("Model::createVBO::color");
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
										std::string(arrayIndex + "attenuation.constant").c_str(),
										light->attenuation.constant);
				Shader::setUniformFloat(shaderIndex,
										std::string(arrayIndex + "attenuation.linear").c_str(),
										light->attenuation.linear);
				Shader::setUniformFloat(shaderIndex,
										std::string(arrayIndex + "attenuation.quadratic").c_str(),
										light->attenuation.quadratic);
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
			}
			Shader::setUniformInt(shaderIndex, "numLights", count);
		}

		void setRenderParams(int shaderIndex, RenderParams* renderParams)
		{
			Shader::setUniformFloat(shaderIndex, "fog.density",  renderParams->fog.density);
			Shader::setUniformFloat(shaderIndex, "fog.start",    renderParams->fog.start);
			Shader::setUniformFloat(shaderIndex, "fog.max",      renderParams->fog.max);
			Shader::setUniformInt(shaderIndex,   "fog.fogMode",  renderParams->fog.fogMode);
			Shader::setUniformVec4(shaderIndex,  "fog.color",    renderParams->fog.color);
			Shader::setUniformVec4(shaderIndex,  "ambientLight", renderParams->ambientLight);
		}
		
		void renderAllModels(CCamera* camera, RenderParams* renderParams)
		{
			for(Mat_Type material : Material::MATERIAL_LIST)
			{
				// TODO: Add error checking for returned material params
				std::vector<int>* registeredMeshes = Material::getRegisteredModels(material);
				int shaderIndex = Material::getShaderIndex(material);

				Shader::bindShader(shaderIndex);

				if((material == MAT_PHONG || material == MAT_PHONG_TEXTURED)
				   && registeredMeshes->size() > 1)
				{
					setLights(shaderIndex);					
					GameObject* viewer = SceneManager::find(camera->node);
					CTransform* viewerTransform = GO::getTransform(viewer);
					Shader::setUniformVec3(shaderIndex, "eyePos", viewerTransform->position);
				}
				setRenderParams(shaderIndex, renderParams);
				
				for(int modelIndex : *registeredMeshes)
				{
					CModel      model      = modelList[modelIndex];
					GameObject* gameObject = SceneManager::find(model.node);
					CTransform* transform  = GO::getTransform(gameObject);

					Mat4 mvp = camera->viewProjMat * transform->transMat;
					Shader::setUniformMat4(shaderIndex, "mvp", mvp);
					if(material == MAT_PHONG || material == MAT_PHONG_TEXTURED)
						Shader::setUniformMat4(shaderIndex, "modelMat", transform->transMat);
					
					Material::setMaterialUniforms(&model.materialUniforms, (Mat_Type)model.material);
					
					glBindVertexArray(model.vao);

					if(model.drawIndexed)
						glDrawElements(GL_TRIANGLES, model.indices.size(), GL_UNSIGNED_INT, (void*)0);
					else
						glDrawArrays(GL_TRIANGLES, 0, model.vertices.size());

					glBindVertexArray(0);

					if(model.material == MAT_UNSHADED_TEXTURED || model.material == MAT_PHONG_TEXTURED)
						Texture::unbindActiveTexture();
				}
				
				Shader::unbindActiveShader();
			}
		}

		int create(CModel* model)
		{
			assert(model);

			// Create VAO for the model that will be used in rendering
			createVAO(model);
			
			int index = -1;
			if(emptyIndices.empty())
			{
				modelList.push_back(*model);
				index = modelList.size() - 1;
			}
			else
			{
				index = emptyIndices.back();
				modelList[index] = *model;
				emptyIndices.pop_back();
			}

			if(Material::registerModel(index, (Mat_Type)model->material))
				Log::message("Model " + model->filename + " registered");
			else
				Log::warning("Model " + model->filename + " already registered!");

			return index;
		}

		CModel* getModelAtIndex(unsigned int modelIndex)
		{
			return &modelList[modelIndex];
		}


		void cleanup()
		{
			free(modelPath);
			for(unsigned int i = 0; i < modelList.size(); i++)
				remove(i);

			modelList.clear();
			emptyIndices.clear();
		}

		// CModel* create(const std::string& filename)
		// {
		// 	// If model is created without a parent node then it cannot be
		// 	// added to gameobjects. It can only be used to provide collision
		// 	// mesh.
			
		// }

		void generateBindings()
		{
			Sqrat::RootTable().Bind("CModel", Sqrat::Class<CModel>()
									.Var("node",             &CModel::node)
									.Var("filename",         &CModel::filename)
									.Var("vertices",         &CModel::vertices)
									.Var("vertexColors",     &CModel::vertexColors)
									.Var("normals",          &CModel::normals)
									.Var("uvs",              &CModel::uvs)
									.Var("indices",          &CModel::indices)
									.Var("material",         &CModel::material)
									.Var("materialUniforms", &CModel::materialUniforms)
									.Var("drawIndexed",      &CModel::drawIndexed));


			Sqrat::RootTable().Bind("Model", Sqrat::Table(ScriptEngine::getVM())
									.Func("create",          &create)
									.Func("remove",          &remove)
									.Func("loadFromFile",    &loadFromFile)
									.Func("setMaterialType", &setMaterialType)
									.Func("getModelAtIndex", &getModelAtIndex));
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
			assert(model);
			assert(filename);
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

				assert(file);

				const uint32_t INDEX_SIZE = sizeof(uint32_t);
				const uint32_t VEC3_SIZE  = sizeof(Vec3);
				const uint32_t VEC2_SIZE  = sizeof(Vec2);
			
				uint32_t header[4];
				size_t bytesRead = 0;
				if((bytesRead = fread(header, INDEX_SIZE, 4, file)) <= 0)
					Log::error("Model::loadFromFile", "Read failed");
				else
				{
					uint32_t indicesCount  = header[0];
					uint32_t verticesCount = header[1];
					uint32_t normalsCount  = header[2];
					uint32_t uvsCount      = header[3];

					// Log::message("IndicesCount : " + std::to_string(indicesCount));
					
					// Indices
					model->indices.reserve(indicesCount);
					model->indices.insert(model->indices.begin(), indicesCount, 0);				
					bytesRead = fread(&model->indices[0], INDEX_SIZE, indicesCount, file);

					// Vertices
					model->vertices.reserve(verticesCount);
					model->vertices.insert(model->vertices.begin(), verticesCount, Vec3(0.f));
					bytesRead = fread(&model->vertices[0], VEC3_SIZE, verticesCount, file);

					// Normals
					model->normals.reserve(normalsCount);
					model->normals.insert(model->normals.begin(), normalsCount, Vec3(0.f));				
					bytesRead = fread(&model->normals[0], VEC3_SIZE, normalsCount, file);

					// UVs
					model->uvs.reserve(uvsCount);
					model->uvs.insert(model->uvs.begin(), uvsCount, Vec2(0.f));				
					bytesRead = fread(&model->uvs[0], VEC2_SIZE, uvsCount, file);					
				}

				fclose(file);
				model->filename = filename;
				model->drawIndexed = true;
			}
			else
			{
				*model = modelList[index];
			}

			return success;
		}

		void setMaterialType(CModel* model, Mat_Type material)
		{
			int index = findModelIndex(model->filename.c_str());
			Material::unRegisterModel(index, (Mat_Type)model->material);
			model->material = material;
			Material::registerModel(index, material);
		}
	}
}

