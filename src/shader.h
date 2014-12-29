#ifndef shader_H
#define shader_H

#include <iostream>

namespace Shader
{
	struct ShaderObject
	{
		unsigned int fragmentShader;
		unsigned int vertexShader;
		unsigned int program; 
	};
	
	int  create(const char* vertexShader, const char* fragmentShader);
	void bindShader(int shaderIndex);
	void unbindActiveShader();
	void destroyShader(unsigned int shaderIndex);
	void cleanup();
}

#endif
