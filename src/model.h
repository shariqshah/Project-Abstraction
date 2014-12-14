#ifndef _model_H_
#define _model_H_

#include "componentTypes.h"
#include "renderer.h"

struct CModel
{
	Resource    resourceID = 0;
	Node        node       = 0;
	std::string filename   = "NOT_INITIALIZED";
	bool        valid      = true;
};

namespace Renderer
{
	namespace Model
	{
		float*  getVertices(CModel* model);
		CModel* create(const std::string& filename);
		int     getVertexCount(CModel* model);
		void    remove(CModel* model);
		void    generateBindings();
		void    cleanup();
	}
}
	
#endif
