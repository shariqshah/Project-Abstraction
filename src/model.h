#ifndef model_H
#define model_H

#include "componentTypes.h"
#include "mathdefs.h"
#include "material.h"
#include "datatypes.h"
#include "jsondefs.h"

struct CCamera;
struct CLight;
struct GameObject;
struct RenderParams;

struct CModel
{
	Node         node          = -1;
	int          material      = 0;
	int          geometryIndex = -1;
	Mat_Uniforms materialUniforms;
};

namespace Model
{
	void    initialize();
	void    renderAllModels(CCamera* camera, RenderParams* renderParams);
	void    renderAllModels(CCamera* camera, RenderParams* renderParams, CLight* light);
	void    renderAllModels(CCamera* camera, CLight* light, int shader);
	CModel* getModelAtIndex(int modelIndex);
	CModel* findModel(const char* filename);
	int     create(const char* filename);
	bool    createFromJSON(CModel* model, const rapidjson::Value& value);
	void    remove(int modelIndex);
	void    generateBindings();
	void    cleanup();
	bool    setMaterialType(CModel* model, Mat_Type material);
	bool    setGeometry(CModel* model, const std::string& filename);
	bool    writeToJSON(CModel* model, rapidjson::Writer<rapidjson::StringBuffer>& writer);
}
	
#endif
