#ifndef _model_H_
#define _model_H_

#include "componentTypes.h"
#include "renderer.h"
#include "mathdefs.h"
#include "material.h"

struct CCamera;

struct CModel
{
	Resource    resourceID = 0;
	Node        node       = 0;
	std::string filename   = "NOT_INITIALIZED";
	bool        valid      = true;
	
	std::vector<Vec3>         vertices;
	std::vector<Vec3>         vertexColors;
	std::vector<Vec3>         normals;
	std::vector<Vec2>         uvs;
	std::vector<unsigned int> indices;
	Mat_Type                  material;
	unsigned int              vboStartIndex;
};

namespace Renderer
{
	namespace Model
	{
		void    initialize();
		void    renderAllModels(CCamera* camera);
		float*  getVertices(CModel* model);
		CModel* create(const std::string& filename);
		int     getVertexCount(CModel* model);
		void    remove(CModel* model);
		void    generateBindings();
		void    cleanup();
	}
}
	
#endif
