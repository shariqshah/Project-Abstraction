#ifndef _model_H_
#define _model_H_

#include "componentTypes.h"
#include "mathdefs.h"
#include "material.h"

struct CCamera;
struct GameObject;
struct RenderParams;

struct CModel
{
	Node                      node     = 0;
	std::string               filename = "UNDEFINED";
	std::vector<Vec3>         vertices;
	std::vector<Vec3>         vertexColors;
	std::vector<Vec3>         normals;
	std::vector<Vec2>         uvs;
	std::vector<unsigned int> indices;
	int                       material = 0;
	unsigned int              vao;
	Mat_Uniforms              materialUniforms;
	bool                      drawIndexed = false;
};

namespace Renderer
{
	namespace Model
	{
		void    initialize(const char* path);
		void    renderAllModels(CCamera* camera, RenderParams* renderParams);
		CModel* getModelAtIndex(unsigned int modelIndex);
		int     create(CModel* model);
		void    remove(unsigned int modelIndex);
		void    generateBindings();
		void    cleanup();
		bool    loadFromFile(const char* filename, CModel* model);
		void    setMaterialType(CModel* model, Mat_Type material);
	}
}
	
#endif
