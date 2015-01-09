#include <GL/glew.h>
#include <GL/gl.h>

#include "model.h"
#include "camera.h"
#include "scriptengine.h"
#include "shader.h"
#include "gameobject.h"
#include "scenemanager.h"
#include "transform.h"
#include "texture.h"

namespace Renderer
{
	namespace Model
	{
		namespace
		{
			std::vector<CModel>        modelList;
			std::vector<unsigned int>  emptyIndices;
			int texture;
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
		}

		void renderAllModels(CCamera* camera)
		{
			for(Mat_Type material : Material::MATERIAL_LIST)
			{
				// TODO: Add error checking for returned material params
				std::vector<int>* registeredMeshes = Material::getRegisteredModels(material);
				int shaderIndex = Material::getShaderIndex(material);

				Shader::bindShader(shaderIndex);
				Texture::bindTexture(texture);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				
				for(int modelIndex : *registeredMeshes)
				{
					CModel      model      = modelList[modelIndex];
					GameObject* gameObject = SceneManager::find(model.node);
					CTransform* transform  = GO::getTransform(gameObject);

					Mat4 mvp = camera->viewProjMat * transform->transMat;
					Shader::setUniformMat4(shaderIndex, "mvp", mvp);
					Material::setMaterialUniforms(&model.materialUniforms, model.material);
					
					glBindVertexArray(model.vao);

					if(model.drawIndexed)
						glDrawElements(GL_TRIANGLES, model.indices.size(), GL_UNSIGNED_INT, (void*)0);
					else
						glDrawArrays(GL_TRIANGLES, 0, model.vertices.size());

					glBindVertexArray(0);
				}
				glDisable(GL_BLEND);
				Texture::unBindActiveTexture();
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

			if(Material::registerModel(index, model->material))
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

			Texture::remove(texture);
		}

		CModel* create(const std::string& filename)
		{
			// If model is created without a parent node then it cannot be
			// added to gameobjects. It can only be used to provide collision
			// mesh.
			
		}

		void generateBindings()
		{
			Sqrat::RootTable().Bind("CModel", Sqrat::Class<CModel>()
								.Var("node",     &CModel::node)
								.Var("filename", &CModel::filename));


			// Sqrat::RootTable().Bind("Model", Sqrat::Table(ScriptEngine::getVM())
			// 					.Func("getVertices", &getVertices)
			// 					// .Func("create", &create)
			// 					.Func("getVertexcount", &getVertexCount));
			// 					// .Func("remove", &remove));
		}

		void remove(unsigned int modelIndex)
		{
			CModel model = modelList[modelIndex];
			glDeleteVertexArrays(1, &model.vao);

			Material::removeMaterialUniforms(&model.materialUniforms, model.material);
			model.vertices.clear();
			model.normals.clear();
			model.uvs.clear();
			model.vertexColors.clear();

			if(!Material::unRegisterModel(modelIndex, model.material))
				Log::warning("Model at index " + std::to_string(modelIndex) + " not unregistered");
			emptyIndices.push_back(modelIndex);
		}
		
		void initialize(const char* path)
		{
			modelPath   = (char *)malloc(sizeof(char) * strlen(path) + 1);
			strcpy(modelPath, path);
			
			texture = Texture::create("test2.png");
		}

		bool loadFromFile(const char* filename, CModel* model)
		{
			assert(model);
			assert(filename);
			bool success = true;

			char* fullPath = (char *)malloc(sizeof(char) *
											(strlen(modelPath) + strlen(filename)) + 1);
			strcpy(fullPath, modelPath);
			strcat(fullPath, filename);
			
			FILE* file = fopen(fullPath, "rb");
			free(fullPath);

			assert(file);

			const uint32_t HEADER_SIZE = sizeof(uint32_t) * 4;
			const uint32_t INDEX_SIZE  = sizeof(uint32_t);
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

				Log::message("IndicesCount : " + std::to_string(indicesCount));

				// size_t indicesPos  = HEADER_SIZE;
				// size_t verticesPos = indicesPos  + (indicesCount  * INDEX_SIZE);
				// size_t normalsPos  = verticesPos + (verticesCount * VEC3_SIZE);
				// size_t uvsPos      = normalsPos  + (normalsCount  * VEC3_SIZE);

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

			return success;
		}
	}
}

