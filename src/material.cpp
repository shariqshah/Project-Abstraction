#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>

#include "material.h"
#include "shader.h"
#include "model.h"
#include "mathdefs.h"
#include "texture.h"
#include "log.h"
#include "scriptengine.h"

namespace Material
{
	struct Material
	{
        std::vector<int>  registeredModels;
		int               shaderIndex;
	};
	
	namespace
	{
		Material unshaded;
		Material phong;
		Material unshadedTextured;
		Material phongTextured;
		const Vec4  DEFAULT_COLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}

	void initialize()
	{
		unshaded.shaderIndex         = Shader::create("unshaded.vert", "unshaded.frag");
		unshadedTextured.shaderIndex = Shader::create("unshaded_textured.vert", "unshaded_textured.frag");
		phong.shaderIndex            = Shader::create("phong.vert", "phong.frag");
		phongTextured.shaderIndex    = Shader::create("phong.vert", "phongTextured.frag");
	}
	
	bool registerModel(int modelIndex, Mat_Type material)
	{
		// Check if model is already registererd, if not register it
		bool exists = false;
		std::vector<int>* registeredNodes = getRegisteredModels(material);
		for(int existingNode : *registeredNodes)
		{
			if(existingNode == modelIndex)
			{
				exists = true;
				break;
			}
		}

		if(!exists)
			registeredNodes->push_back(modelIndex);

		// if node is already registered, return false otherwise true
		return exists ? false : true;
	}
	

	int getShaderIndex(Mat_Type material)
	{
		int shaderIndex = -1;
		
		switch(material)
		{
		case MAT_UNSHADED:	        shaderIndex = unshaded.shaderIndex;	          break;
		case MAT_UNSHADED_TEXTURED:	shaderIndex = unshadedTextured.shaderIndex;	  break;
		case MAT_PHONG:			    shaderIndex = phong.shaderIndex;			  break;
		case MAT_PHONG_TEXTURED:	shaderIndex = phongTextured.shaderIndex;	  break;
		default: Log::error("Material::getShaderIndex", "Invalid Material type"); break;
		};

		return shaderIndex;
	}

	bool unRegisterModel(int modelIndex, Mat_Type material)
	{
		int  index = -1;
		bool found = false;
		std::vector<int>* registeredNodes = getRegisteredModels(material);

		for(int registeredNode : *registeredNodes)
		{
			index++;
			if(registeredNode == modelIndex)
			{
				found = true;
				break;
			}
		}

		if(found)
			registeredNodes->erase(registeredNodes->begin() + index);

		return found;
	}
	
	std::vector<int>* getRegisteredModels(Mat_Type material)
    {
		std::vector<int>* registeredModels = NULL;
		
		switch(material)
		{
        case MAT_UNSHADED:	        registeredModels = &unshaded.registeredModels;	      break;
        case MAT_UNSHADED_TEXTURED:	registeredModels = &unshadedTextured.registeredModels; break;
        case MAT_PHONG:			    registeredModels = &phong.registeredModels;  		  break;
        case MAT_PHONG_TEXTURED:	registeredModels = &phongTextured.registeredModels;    break;
		default: Log::error("Material::getRegisteredModels", "Invalid Material type");	  break;
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
		case MAT_UNSHADED: break;
		case MAT_UNSHADED_TEXTURED:	Texture::remove(materialUniforms->texture);	break;
		case MAT_PHONG:	break;
		case MAT_PHONG_TEXTURED:	Texture::remove(materialUniforms->texture);	break;
		default: Log::error("Material::removeMaterialUniforms", "Invalid Material type"); break;
		}
	}

	void cleanup()
	{
        unshaded.registeredModels.clear();
        unshadedTextured.registeredModels.clear();
        phong.registeredModels.clear();
        phongTextured.registeredModels.clear();
	}

	void generateBindings()
	{
		Sqrat::ConstTable().Enum("Mat_Type", Sqrat::Enumeration ()
								 .Const("UNSHADED",          Mat_Type::MAT_UNSHADED)
								 .Const("UNSHADED_TEXTURED", Mat_Type::MAT_UNSHADED_TEXTURED)
								 .Const("PHONG",             Mat_Type::MAT_PHONG)
								 .Const("PHONG_TEXTURED",    Mat_Type::MAT_PHONG_TEXTURED));
		
		Sqrat::RootTable().Bind("Mat_Uniforms", Sqrat::Class<Mat_Uniforms>()
								.Var("diffuseColor",     &Mat_Uniforms::diffuseColor)
								.Var("diffuse",          &Mat_Uniforms::diffuse)
								.Var("specular",         &Mat_Uniforms::specular)
								.Var("specularStrength", &Mat_Uniforms::specularStrength)
								.Var("texture",          &Mat_Uniforms::texture));
	} 
}
