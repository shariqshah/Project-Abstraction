#include <GL/glew.h>
#include <GL/gl.h>
#include <string.h>
#include <assert.h>
#include <vector>

#include "shader.h"
#include "utilities.h"
#include "log.h"

namespace Shader
{
	namespace
	{
		std::vector<ShaderObject> shaderObjects;
		std::vector<unsigned int> emptyIndices;
	}
	
	int create(const char* vertexShader, const char* fragmentShader)
	{
		GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

		char* vertSource = Utils::loadFileIntoCString(vertexShader);
		char* fragSource = Utils::loadFileIntoCString(fragmentShader);

		assert(vertSource != NULL);
		assert(fragSource != NULL);
		
		GLint vSize = (GLint)strlen(vertSource);
		GLint fSize = (GLint)strlen(fragSource);
		
		const char* vertSourcePtr = vertSource;
		const char* fragSourcePtr = fragSource;
		
		const GLint* vertSize = &vSize;
		const GLint* fragSize = &fSize;
		
		glShaderSource(vertShader, 1, &vertSourcePtr, vertSize);
		glShaderSource(fragShader, 1, &fragSourcePtr, fragSize);

	    glCompileShader(vertShader);
	    glCompileShader(fragShader);

		free(vertSource);
		free(fragSource);
		
		GLint isVertCompiled = 0;
		GLint isFragCompiled = 0;
		glGetShaderiv(vertShader, GL_COMPILE_STATUS, &isVertCompiled);
		glGetShaderiv(fragShader, GL_COMPILE_STATUS, &isFragCompiled);

		if(!isVertCompiled)
		{
			GLint logSize = 0;
			glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logSize);
			char* message = (char *)malloc(sizeof(char) * logSize);
			glGetShaderInfoLog(vertShader, logSize, NULL, message);

			Log::error("COMPILE " + std::string(vertexShader), std::string(message));
			free(message);
		}

		if(!isFragCompiled)
		{
			GLint logSize = 0;
			glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logSize);
			char* message = (char *)malloc(sizeof(char) * logSize);
			glGetShaderInfoLog(fragShader, logSize, NULL, message);

			Log::error("COMPILE " + std::string(fragmentShader), std::string(message));
			free(message);
		}

		if(!isVertCompiled || !isFragCompiled)
		{
			glDeleteShader(vertShader);
			glDeleteShader(fragShader);
			return -1;
		}

		GLuint program = glCreateProgram();
		glAttachShader(program, vertShader);
		glAttachShader(program, fragShader);

		// Bind attribute locations
		glBindAttribLocation(program, 0, "vPosition");
		glBindAttribLocation(program, 1, "vNormal");
		glBindAttribLocation(program, 2, "vUV");

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

		if(!isLinked)
		{
			GLint logSize = 0;
			glGetProgramiv(program, GL_LINK_STATUS, &logSize);
			char* message = (char *)malloc(sizeof(char) * logSize);
			glGetProgramInfoLog(program, logSize, NULL, message);
			Log::error("LINK SHADER", std::string(message));
			free(message);

			glDeleteProgram(program);
			glDeleteShader(vertShader);
			glDeleteShader(fragShader);
			
			return -1;
		}

		ShaderObject newObject;
		newObject.vertexShader   = vertShader;
		newObject.fragmentShader = fragShader;
		newObject.program        = program;
		
		// Find index
		int index = -1;
		if(!emptyIndices.empty())
		{
			index = emptyIndices.back();
			emptyIndices.pop_back();
			shaderObjects[index] = newObject;
		}
		else
		{
			shaderObjects.push_back(newObject);
			index = shaderObjects.size() - 1;
		}

		Log::message(std::string(vertexShader) + ", " + std::string(fragmentShader) +
					 " compiled into shader program");
		return index;
	}

	void bindShader(int shaderIndex)
	{
		auto shaderObject = shaderObjects[shaderIndex];
		glUseProgram(shaderObject.program);
	}

	void unbindActiveShader()
	{
		glUseProgram(0);
	}

	void destroyShader(int shaderIndex)
	{
		auto shaderObject = shaderObjects[shaderIndex];
		glDeleteProgram(shaderObject.program);
		glDeleteShader(shaderObject.vertexShader);
		glDeleteShader(shaderObject.fragmentShader);
		emptyIndices.push_back(shaderIndex);
	}
	
	void cleanup()
	{
		for(int i = 0; i < shaderObjects.size(); i++)
			destroyShader(i);

		shaderObjects.clear();
		emptyIndices.clear();
	}
}
