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
    
	int  create(const char* vertexShader, const char* fragmentShader);
	void initialize(const char* path);
	void bindShader(const unsigned int shaderIndex);
	void remove(const unsigned int shaderIndex);
	void unbindActiveShader();
	void setUniformInt(const unsigned int shaderIndex, const char* name, const int value);
	void setUniformFloat(const unsigned int shaderIndex, const char* name, const float value);
	void setUniformVec2(const unsigned int shaderIndex,  const char* name, const Vec2 value);
	void setUniformVec3(const unsigned int shaderIndex,  const char* name, const Vec3 value);
	void setUniformVec4(const unsigned int shaderIndex,  const char* name, const Vec4 value);
	void setUniformMat4(const unsigned int shaderIndex,  const char* name, const Mat4 value);
	int  getUniformLocation(const unsigned int shaderIndex, const char* name);
	void cleanup();
}

#endif
