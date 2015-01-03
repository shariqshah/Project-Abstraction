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
		std::vector<int>  registeredNodes;
		Vec3              diffuseColor;
		int               shaderIndex;
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

	bool registerModel(int modelIndex, Mat_Type material)
	{
		// Check if model is already registererd, if not register it
		bool exists = false;
		switch(material)
		{
		case MAT_UNSHADED:

			for(int existingNode : matUnshaded.registeredNodes)
			{
				if(existingNode == modelIndex)
				{
					exists = true;
					break;
				}
			}
			
			if(!exists)
				matUnshaded.registeredNodes.push_back(modelIndex);
			
			break;
		case MAT_UNSHADED_TEXTURED:
			Log::error("Model::registerModel", "Material type unimplemented");
			break;
		case MAT_PHONG:
			Log::error("Model::registerModel", "Material type unimplemented");
			break;
		case MAT_PHONG_TEXTURED:
			Log::error("Model::registerModel", "Material type unimplemented");
			break;
		default:
			Log::error("Model::registerModel", "Invalid Material type");
			break;
		}

		// if node is already registered, return false otherwise true
		return exists ? false : true;
	}
	

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

	bool unRegisterModel(int modelIndex, Mat_Type material)
	{
		int index = -1;
		switch(material)
		{
		case MAT_UNSHADED:
			for(unsigned int i = 0; i < matUnshaded.registeredNodes.size(); i++)
			{
				if(matUnshaded.registeredNodes[i] == modelIndex)
				{
					index = i;
					break;
				}
			}

			if(index != -1)
				matUnshaded.registeredNodes.erase(matUnshaded.registeredNodes.begin() + index);
			
			break;
		case MAT_UNSHADED_TEXTURED:
			Log::error("Model::unRegisterModel", "Material type unimplemented");
			break;
		case MAT_PHONG:
			Log::error("Model::unRegisterModel", "Material type unimplemented");
			break;
		case MAT_PHONG_TEXTURED:
			Log::error("Model::unRegisterModel", "Material type unimplemented");
			break;
		default:
			Log::error("Model::unRegisterModel", "Invalid Material type");
			break;
		};

		return index != -1 ? true : false;
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

	void cleanup()
	{
		matUnshaded.registeredNodes.clear();
	}
}
