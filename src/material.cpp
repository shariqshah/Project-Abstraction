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
		const int   MAX_VERTICES        = 1000000;
		const int   MAX_VERT_VBO_SIZE   = sizeof(Vec3) * MAX_VERTICES;
		const int   MAX_COLOR_VBO_SIZE  = sizeof(Vec3) * MAX_VERTICES;
		const int   MAX_UV_VBO_SIZE     = sizeof(Vec2) * MAX_VERTICES;
		const int   MAX_NORMAL_VBO_SIZE = sizeof(Vec3) * MAX_VERTICES;
		const int   MAX_INDEX_VBO_SIZE  = MAX_VERTICES / 3;
	}

	void initialize()
	{
		matUnshaded.shaderIndex = Shader::create("unshaded.vert", "unshaded.frag");
	}

	void registerModel(/* Node or Model, Mat_Type, Mat_Uniforms*/); //TODO: Figure this out!!!!!
	// void setMaterialUniform(Mat_Type type, /* value to set use overloaded function of same name or gl-style i,f,v3,v4,mat prefixes*/);
}
