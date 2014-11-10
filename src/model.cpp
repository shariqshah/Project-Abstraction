#include "model.h"


namespace Renderer
{
	namespace Model
	{
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

		void remove(const CModel& model)
		{
			if(model.resourceID != 0 &&
			   Renderer::Resources::isLoaded(model.resourceID))
			{
				if(!Renderer::Resources::remove(model.resourceID))
					Log::warning("Resource not removed for Model!");

				if(model.node != 0)
				{
					if(!Renderer::removeNode(model.node))
						Log::warning("Node not removed for Model");
				}
			}
		}

		CModel* create(const std::string& filename)
		{
			// If model is created without a parent node then it cannot be
			// added to gameobjects. It can only be used to provide collision
			// mesh.
			
			CModel* newModel = new CModel;
			
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
			}

			newModel->valid = false;
			return newModel;
		}
	}
}

