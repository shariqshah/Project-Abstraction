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
#include "passert.h"

// Overloaded assignment operator for Mat_Uniform
Mat_Uniforms& Mat_Uniforms::operator=(const Mat_Uniforms& other)
{
	if(&other != this)			// If assigning self, just return
	{
		this->diffuseColor     = other.diffuseColor;
		this->diffuse          = other.diffuse;
		this->specular         = other.specular;
		this->specularStrength = other.specularStrength;
		if(this->texture != -1)
			Texture::decreaseRefCount(this->texture);
		if(other.texture != -1)
			Texture::increaseRefCount(other.texture);

		// *this = other;
		this->texture = other.texture;
	}
	return *this;
}

namespace Material
{
	struct Material
	{
        std::vector<int> registeredModels;
		int              shaderIndex;
	};
	
	namespace
	{
		Material unshaded;
		Material phong;
		Material unshadedTextured;
		Material phongTextured;
		const Vec4 DEFAULT_COLOR(1.0f, 1.0f, 1.0f, 1.0f);
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
        case MAT_UNSHADED:	        registeredModels = &unshaded.registeredModels;	       break;
        case MAT_UNSHADED_TEXTURED:	registeredModels = &unshadedTextured.registeredModels; break;
        case MAT_PHONG:			    registeredModels = &phong.registeredModels;  		   break;
        case MAT_PHONG_TEXTURED:	registeredModels = &phongTextured.registeredModels;    break;
		default: Log::error("Material::getRegisteredModels", "Invalid Material type");	   break;
		};

		return registeredModels;
	}

	void setMaterialUniforms(const Mat_Uniforms* materialUniforms, Mat_Type material)
	{
		PA_ASSERT(materialUniforms);
		unsigned int shaderIndex = getShaderIndex(material);
		// Set diffuse color
		Shader::setUniformVec4(shaderIndex, "diffuseColor", materialUniforms->diffuseColor);
		// Bind texture if applicable
		if(material == MAT_UNSHADED_TEXTURED || material == MAT_PHONG_TEXTURED)
		{
			Shader::setUniformInt(shaderIndex, "sampler", (GL_TEXTURE0 + 4) - GL_TEXTURE0);
			Texture::bind(materialUniforms->texture, 4);
		}

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
		PA_ASSERT(materialUniforms);

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
		asIScriptEngine* engine = ScriptEngine::getEngine();
		int rc = engine->RegisterEnum("Mat_Type");
		rc = engine->RegisterEnumValue("Mat_Type", "UNSHADED", (int)Mat_Type::MAT_UNSHADED);
		rc = engine->RegisterEnumValue("Mat_Type", "UNSHADED_TEXTURED", (int)Mat_Type::MAT_UNSHADED_TEXTURED);
		rc = engine->RegisterEnumValue("Mat_Type", "PHONG", (int)Mat_Type::MAT_PHONG);
		rc = engine->RegisterEnumValue("Mat_Type", "PHONG_TEXTURED", (int)Mat_Type::MAT_PHONG_TEXTURED);

		rc = engine->RegisterObjectType("Mat_Uniforms", sizeof(Mat_Uniforms), asOBJ_REF | asOBJ_NOCOUNT);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Mat_Uniforms",
											"Vec4 diffuseColor",
											asOFFSET(Mat_Uniforms, diffuseColor));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Mat_Uniforms",
											"float specular",
											asOFFSET(Mat_Uniforms, specular));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Mat_Uniforms",
											"float specularStrength",
											asOFFSET(Mat_Uniforms, specularStrength));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Mat_Uniforms",
											"float diffuse",
											asOFFSET(Mat_Uniforms, diffuse));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Mat_Uniforms",
											"int texture",
											asOFFSET(Mat_Uniforms, texture));
		rc = engine->RegisterObjectProperty("Mat_Uniforms",
											"bool castShadow",
											asOFFSET(Mat_Uniforms, castShadow));
		PA_ASSERT(rc >= 0);
	} 
}
