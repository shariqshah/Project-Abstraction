#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>

#include "material.h"
#include "shader.h"
#include "model.h"
#include "mathdefs.h"

namespace Material
{
	struct MatUnshaded
	{
		std::vector<int> registeredNodes;
		Vec3             diffuseColor;
		int              shaderIndex;
	};
	
	namespace
	{
		MatUnshaded matUnshaded;
		const Vec4  DEFAULT_COLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}

	void initialize()
	{
		matUnshaded.shaderIndex = Shader::create("unshaded.vert", "unshaded.frag");
	}

	void registerModel(/* Node or Model, Mat_Type, Mat_Uniforms*/); //TODO: Figure this out!!!!!
	// void setMaterialUniform(Mat_Type type, /* value to set use overloaded function of same name or gl-style i,f,v3,v4,mat prefixes*/);

	int getShaderIndex(Mat_Type material)
	{
		int shaderIndex = -1;
		
		switch(material)
		{
		case MAT_UNSHADED:
			shaderIndex = matUnshaded.shaderIndex;
			break;
		case MAT_UNSHADED_TEXTURED:
			Log::error("Model::getShaderIndex", "Material type unimplemented");
			break;
		case MAT_PHONG:
			Log::error("Model::getShaderIndex", "Material type unimplemented");
			break;
		case MAT_PHONG_TEXTURED:
			Log::error("Model::getShaderIndex", "Material type unimplemented");
			break;
		default:
			Log::error("Model::getShaderIndex", "Invalid Material type");
			break;
		};

		return shaderIndex;
	}

	std::vector<int>* getRegisteredModels(Mat_Type material)
    {
		std::vector<int>* registeredModels = NULL;
		
		switch(material)
		{
		case MAT_UNSHADED:
			registeredModels = &matUnshaded.registeredNodes;
			break;
		case MAT_UNSHADED_TEXTURED:
			Log::error("Model::getRegisteredModels", "Material type unimplemented");
			break;
		case MAT_PHONG:
			Log::error("Model::getRegisteredModels", "Material type unimplemented");
			break;
		case MAT_PHONG_TEXTURED:
			Log::error("Model::getRegisteredModels", "Material type unimplemented");
			break;
		default:
			Log::error("Model::getRegisteredModels", "Invalid Material type");
			break;
		};

		return registeredModels;
	}
}
