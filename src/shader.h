#ifndef shader_H
#define shader_H

#include "mathdefs.h"

namespace Shader
{
	// Constants for locations of attributes inside all shaders
	const int POSITION_LOC = 0;
	const int NORMAL_LOC   = 1;
	const int UV_LOC       = 2;
	const int COLOR_LOC    = 3;
    
	int  create(const char* vertexShaderName, const char* fragmentShaderName);
	void initialize(const char* path);
	void bind(const int shaderIndex);
	void remove(const int shaderIndex);
	void unbind();
	void setUniformInt(const int shaderIndex, const char* name, const int value);
	void setUniformFloat(const int shaderIndex, const char* name, const float value);
	void setUniformVec2(const int shaderIndex,  const char* name, const Vec2 value);
	void setUniformVec3(const int shaderIndex,  const char* name, const Vec3 value);
	void setUniformVec4(const int shaderIndex,  const char* name, const Vec4 value);
	void setUniformMat4(const int shaderIndex,  const char* name, const Mat4 value);
	int  getUniformLocation(const int shaderIndex, const char* name);
	void cleanup();
}

#endif
