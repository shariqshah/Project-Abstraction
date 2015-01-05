#ifndef _model_H_
#define _model_H_

#include "componentTypes.h"
#include "renderer.h"
#include "mathdefs.h"
#include "material.h"

struct CCamera;
struct GameObject;

struct CModel
{
	Node                      node     = 0;
	std::string               filename = "UNDEFINED";
	std::vector<Vec3>         vertices;
	std::vector<Vec3>         vertexColors;
	std::vector<Vec3>         normals;
	std::vector<Vec2>         uvs;
	std::vector<unsigned int> indices;
	Mat_Type                  material;
	unsigned int              vao;
	Mat_Uniforms              materialUniforms;
	bool                      drawIndexed = false;
};

namespace Renderer
{
	namespace Model
	{
		void    initialize();
		void    renderAllModels(CCamera* camera);
		CModel* getModelAtIndex(unsigned int modelIndex);
		CModel* create(const std::string& filename);
		int     create(CModel* model);
		void    remove(CModel* model);
		void    remove(unsigned int modelIndex);
		void    generateBindings();
		void    cleanup();
	}
}
	
#endif
