#include <GL/glew.h>
#include <GL/gl.h>

#include "model.h"
#include "camera.h"
#include "scriptengine.h"
#include "shader.h"
#include "gameobject.h"
#include "scenemanager.h"
#include "componentmanager.h"
#include "transform.h"

namespace Renderer
{
	namespace Model
	{
		namespace
		{
			std::vector<CModel*> untrackedModels;
			std::vector<CModel>  modelList;
			std::vector<unsigned int>  emptyIndices;

			const int    MAX_VERTICES        = 1000000;
			const int    MAX_VERT_VBO_SIZE   = sizeof(Vec3) * MAX_VERTICES;
			const int    MAX_COLOR_VBO_SIZE  = sizeof(Vec3) * MAX_VERTICES;
			const int    MAX_UV_VBO_SIZE     = sizeof(Vec2) * MAX_VERTICES;
			const int    MAX_NORMAL_VBO_SIZE = sizeof(Vec3) * MAX_VERTICES;
			const int    MAX_INDEX_VBO_SIZE  = MAX_VERTICES / 3;
			GLuint       vao;
			unsigned int vertexVBOTop = 0;
			unsigned int colorVBOTop  = 0;
			unsigned int uvVBOTop     = 0;
			unsigned int normalVBOTop = 0;
			unsigned int indexVBOTop  = 0;

			void createVAO(CModel* model)
			{
				// TODO : Add support for indices
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
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

				if(model->normals.size() > 0)
				{
					GLuint normalVBO;
					glGenBuffers(1, &normalVBO);
					glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
					glBufferData(GL_ARRAY_BUFFER,
								 model->normals.size() * sizeof(Vec3),
								 &model->normals,
								 GL_STATIC_DRAW);
					glEnableVertexAttribArray(0);
					glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, 0);
				}

				if(model->uvs.size() > 0)
				{
					GLuint uvVBO;
					glGenBuffers(1, &uvVBO);
					glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
					glBufferData(GL_ARRAY_BUFFER,
								 model->uvs.size() * sizeof(Vec2),
								 &model->uvs,
								 GL_STATIC_DRAW);
					glEnableVertexAttribArray(2);
					glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 0, 0);
				}

				if(model->vertexColors.size() > 0)
				{
					GLuint colorVBO;
					glGenBuffers(1, &colorVBO);
					glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
					glBufferData(GL_ARRAY_BUFFER,
								 model->vertexColors.size() * sizeof(Vec3),
								 &model->vertexColors,
								 GL_STATIC_DRAW);
					glEnableVertexAttribArray(0);
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
				
				for(int modelIndex : *registeredMeshes)
				{
					CModel      model      = modelList[modelIndex];
					GameObject* gameObject = SceneManager::find(model.node);
					CTransform* transform  = CompManager::getTransform(gameObject);

					Mat4 mvp = camera->viewProjMat * transform->transMat;
					Shader::setUniformMat4(shaderIndex, "mvp", mvp);

					glBindVertexArray(model.vao);

					if(model.drawIndexed)
						glDrawElements(GL_TRIANGLES, model.indices.size(), GL_UNSIGNED_INT, (void*)0);
					else
						glDrawArrays(GL_TRIANGLES, 0, model.vertices.size());

					glBindVertexArray(0);
				}

				Shader::unbindActiveShader();
			}
		}

		CModel* addModel(GameObject* gameObject, CModel* model)
		{
			assert(gameObject != NULL);
			assert(model      != NULL);

			model->node = gameObject->node;

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

			gameObject->compIndices[(int)Component::MODEL] = index;
			Log::message("Model added to " + gameObject->name);
			return &modelList[index];
		}


		void cleanup()
		{
			for(CModel* model : untrackedModels)
			{
				if(model != NULL)
				{
					remove(model);
					delete model;
					model = NULL;
				}
			}

			untrackedModels.clear();

			for(unsigned int i = 0; i < modelList.size(); i++)
				remove(i);

			modelList.clear();
			emptyIndices.clear();
		}
		
		float* getVertices(CModel* model)
		{
			Resource geo = h3dGetNodeParamI(model->node, H3DModel::GeoResI);
			float* vertices = (float *)h3dMapResStream(geo,
													   H3DGeoRes::GeometryElem,
													   0,
													   H3DGeoRes::GeoVertPosStream,
													   true,
													   false);

			h3dUnmapResStream(geo);
			return vertices;
		}

		int getVertexCount(CModel* model)
		{
			Resource geo = h3dGetNodeParamI(model->node, H3DModel::GeoResI);
			int vertexCount = h3dGetResParamI(geo,
											  H3DGeoRes::GeometryElem,
											  0,
											  H3DGeoRes::GeoVertexCountI);
			return vertexCount;
		}

		void remove(CModel* model)
		{
			if(model->resourceID != 0 &&
			   Renderer::Resources::isLoaded(model->resourceID))
			{
				if(!Renderer::Resources::remove(model->resourceID))
					Log::warning("Resource not removed for Model!");

				if(model->node != 0)
				{
					if(!Renderer::removeNode(model->node))
						Log::warning("Node not removed for Model");
				}
			}
		}

		CModel* create(const std::string& filename)
		{
			// If model is created without a parent node then it cannot be
			// added to gameobjects. It can only be used to provide collision
			// mesh.
			
			untrackedModels.push_back(new CModel);
			CModel* newModel = untrackedModels.back();
			
			newModel->filename = filename;
			newModel->resourceID = Renderer::Resources::get(ResourceType::MODEL,
															newModel->filename);

			if(newModel->resourceID == 0)
				Log::warning("Model resource could not be created. Check renderer logs.");
			else
			{
				newModel->node = Renderer::createNode(newModel->resourceID,
													  Renderer::ROOT_NODE);

				if(newModel->node == 0)
					Log::warning("Model resource could not be parented to Root. Check renderer logs.");
				else
					Renderer::setNodeFlags(newModel->node, NodeFlag::INACTIVE);
			}

			newModel->valid = false;
			return newModel;
		}

		void generateBindings()
		{
			Sqrat::RootTable().Bind("CModel", Sqrat::Class<CModel>()
								.Var("node",     &CModel::node)
								.Var("filename", &CModel::filename)
								.Var("resource", &CModel::resourceID));

			Sqrat::RootTable().Bind("Model", Sqrat::Table(ScriptEngine::getVM())
								.Func("getVertices", &getVertices)
								.Func("create", &create)
								.Func("getVertexcount", &getVertexCount));
								// .Func("remove", &remove));
		}

		void remove(unsigned int modelIndex)
		{
			CModel model = modelList[modelIndex];
			glDeleteVertexArrays(1, &model.vao);

			model.vertices.clear();
			model.normals.clear();
			model.uvs.clear();
			model.vertexColors.clear();

			if(!Material::unRegisterModel(modelIndex, model.material))
				Log::warning("Model at index " + std::to_string(modelIndex) + " not unregistered");
			emptyIndices.push_back(modelIndex);
		}
		
		void initialize()
		{
			
		}

	}
}

