#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>

#include "material.h"
#include "shader.h"
#include "model.h"
#include "mathdefs.h"
#include "texture.h"
#include "log.h"

namespace Material
{
	struct Material
	{
		std::vector<int>  registeredNodes;
		int               shaderIndex;
	};
	
	namespace
	{
		Material unshaded;
		Material unshadedTextured;
		Material phong;
		const Vec4  DEFAULT_COLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}

	void initialize()
	{
		unshaded.shaderIndex = Shader::create("unshaded.vert", "unshaded.frag");
		unshadedTextured.shaderIndex = Shader::create("unshaded_textured.vert", "unshaded_textured.frag");
		phong.shaderIndex = Shader::create("phong.vert", "phong.frag");
	}

	bool registerModel(int modelIndex, Mat_Type material)
	{
		// Check if model is already registererd, if not register it
		bool exists = false;
		switch(material)
		{
		case MAT_UNSHADED:

			for(int existingNode : unshaded.registeredNodes)
			{
				if(existingNode == modelIndex)
				{
					exists = true;
					break;
				}
			}
			
			if(!exists)
				unshaded.registeredNodes.push_back(modelIndex);
			
			break;
		case MAT_UNSHADED_TEXTURED:
			for(int existingNode : unshadedTextured.registeredNodes)
			{
				if(existingNode == modelIndex)
				{
					exists = true;
					break;
				}
			}
			
			if(!exists)
				unshadedTextured.registeredNodes.push_back(modelIndex);
			break;
		case MAT_PHONG:
			for(int existingNode : phong.registeredNodes)
			{
				if(existingNode == modelIndex)
				{
					exists = true;
					break;
				}
			}
			
			if(!exists)
				phong.registeredNodes.push_back(modelIndex);
			break;
		case MAT_PHONG_TEXTURED:
			Log::error("Material::registerModel", "Material type unimplemented");
			break;
		default:
			Log::error("Material::registerModel", "Invalid Material type");
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
			shaderIndex = unshaded.shaderIndex;
			break;
		case MAT_UNSHADED_TEXTURED:
			shaderIndex = unshadedTextured.shaderIndex;
			break;
		case MAT_PHONG:
			shaderIndex = phong.shaderIndex;
			break;
		case MAT_PHONG_TEXTURED:
			Log::error("Material::getShaderIndex", "Material type unimplemented");
			break;
		default:
			Log::error("Material::getShaderIndex", "Invalid Material type");
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
			for(unsigned int i = 0; i < unshaded.registeredNodes.size(); i++)
			{
				if(unshaded.registeredNodes[i] == modelIndex)
				{
					index = i;
					break;
				}
			}

			if(index != -1)
				unshaded.registeredNodes.erase(unshaded.registeredNodes.begin() + index);
			
			break;
		case MAT_UNSHADED_TEXTURED:
			for(unsigned int i = 0; i < unshadedTextured.registeredNodes.size(); i++)
			{
				if(unshadedTextured.registeredNodes[i] == modelIndex)
				{
					index = i;
					break;
				}
			}

			if(index != -1)
				unshadedTextured.registeredNodes.erase(unshadedTextured.registeredNodes.begin() + index);
			break;
		case MAT_PHONG:
			for(unsigned int i = 0; i < phong.registeredNodes.size(); i++)
			{
				if(phong.registeredNodes[i] == modelIndex)
				{
					index = i;
					break;
				}
			}

			if(index != -1)
				phong.registeredNodes.erase(phong.registeredNodes.begin() + index);
			break;
		case MAT_PHONG_TEXTURED:
			Log::error("Material::unRegisterModel", "Material type unimplemented");
			break;
		default:
			Log::error("Material::unRegisterModel", "Invalid Material type");
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
			registeredModels = &unshaded.registeredNodes;
			break;
		case MAT_UNSHADED_TEXTURED:
			registeredModels = &unshadedTextured.registeredNodes;
			break;
		case MAT_PHONG:
			registeredModels = &phong.registeredNodes;
			break;
		case MAT_PHONG_TEXTURED:
			Log::error("Material::getRegisteredModels", "Material type unimplemented");
			break;
		default:
			Log::error("Material::getRegisteredModels", "Invalid Material type");
			break;
		};

		return registeredModels;
	}

	void setMaterialUniforms(const Mat_Uniforms* materialUniforms, Mat_Type material)
	{
		assert(materialUniforms);

		unsigned int shaderIndex = getShaderIndex(material);

		// Set diffuse color
		Shader::setUniformVec4(shaderIndex, "diffuseColor", materialUniforms->diffuseColor);

		// Bind texture if applicable
		if(material == MAT_UNSHADED_TEXTURED || material == MAT_PHONG_TEXTURED)
			Texture::bindTexture(materialUniforms->texture);

		if(material == MAT_PHONG || material == MAT_PHONG_TEXTURED)
		{
			Shader::setUniformFloat(shaderIndex, "material.specular", materialUniforms->specular);
			Shader::setUniformFloat(shaderIndex, "material.diffuse", materialUniforms->diffuse);
			Shader::setUniformFloat(shaderIndex,
									"material.specularStrength",
									materialUniforms->specularStrength);
		}
	}

	void removeMaterialUniforms(const Mat_Uniforms* materialUniforms, Mat_Type material)
	{
		assert(materialUniforms);

		switch(material)
		{
		case MAT_UNSHADED:
			break;
		case MAT_UNSHADED_TEXTURED:
			Texture::remove(materialUniforms->texture);
			break;
		case MAT_PHONG:
			break;
		case MAT_PHONG_TEXTURED:
			Texture::remove(materialUniforms->texture);
			break;
		default:
			Log::error("Material::removeMaterialUniforms", "Invalid Material type");
			break;
		}
	}

	void cleanup()
	{
		unshaded.registeredNodes.clear();
		unshadedTextured.registeredNodes.clear();
	}
}
