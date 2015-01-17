#ifndef material_H
#define material_H

#include <vector>
#include <iostream>

#include "mathdefs.h"

typedef uint32_t Node;

enum Mat_Type
{
	MAT_UNSHADED = 0,
	MAT_UNSHADED_TEXTURED,
	MAT_PHONG,
	MAT_PHONG_TEXTURED		
};

struct Mat_Uniforms
{
	Vec4  diffuseColor      = Vec4(1.f);
	float diffuse           = 1.f;
	float specular          = 1.f;
	float specularIntensity = 50.f;
	int   texture           = -1;
};

namespace Material
{
	const static std::vector<Mat_Type> MATERIAL_LIST = { MAT_UNSHADED,
														 MAT_UNSHADED_TEXTURED};
														 // MAT_PHONG,
														 // MAT_PHONG_TEXTURED };
	
	void              initialize();
	void              cleanup();
	std::vector<int>* getRegisteredModels(Mat_Type material);
    int               getShaderIndex(Mat_Type material);
	bool              registerModel(int modelIndex, Mat_Type material);
	bool              unRegisterModel(int modelIndex, Mat_Type material);
	void              setMaterialUniforms(const Mat_Uniforms* materialUniforms, Mat_Type material);
	void              removeMaterialUniforms(const Mat_Uniforms* materialUniforms, Mat_Type material);
}

#endif
