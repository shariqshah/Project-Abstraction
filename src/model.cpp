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
		}

		void renderAllModels(CCamera* camera)
		{
			glBindVertexArray(vao);

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

					Mat4 mvp = camera->projectionMat * camera->viewMat * transform->transMat;
					Shader::setUniformMat4(shaderIndex, "mvp", mvp);
					
					glDrawArrays(GL_TRIANGLES,
								 model.vboStartIndex,
								 model.vertices.size());
				}

				Shader::unbindActiveShader();
			}
		    
			glBindVertexArray(0);
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
								.Func("getVertexcount", &getVertexCount)
								.Func("remove", &remove));
		}

		void initialize()
		{
			glBindVertexArray(vao);

			GLuint vertexVBO;
			glGenBuffers(1, &vertexVBO);
			glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
			glBufferData(GL_ARRAY_BUFFER,
						 MAX_VERT_VBO_SIZE,
						 NULL,
						 GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

			GLuint normalVBO;
			glGenBuffers(1, &normalVBO);
			glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
			glBufferData(GL_ARRAY_BUFFER,
						 MAX_NORMAL_VBO_SIZE,
						 NULL,
						 GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, 0);

			GLuint uvVBO;
			glGenBuffers(1, &uvVBO);
			glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
			glBufferData(GL_ARRAY_BUFFER,
						 MAX_UV_VBO_SIZE,
						 NULL,
						 GL_STATIC_DRAW);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 0, 0);

			GLuint colorVBO;
			glGenBuffers(1, &colorVBO);
			glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
			glBufferData(GL_ARRAY_BUFFER,
						 MAX_COLOR_VBO_SIZE,
						 NULL,
						 GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
		
			glBindVertexArray(0);
		}

	}
}

