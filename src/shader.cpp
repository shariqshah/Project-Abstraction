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
	struct ShaderObject
	{
		// TODO: Add name field to shader
		unsigned int fragmentShader;		unsigned int vertexShader;
		unsigned int program; 
	};
	
	namespace
	{
		std::vector<ShaderObject> shaderList;
		std::vector<unsigned int> emptyIndices;
		char*                     shaderPath;
	}

	void initialize(const char* path)
	{
		shaderPath   = (char *)malloc(sizeof(char) * strlen(path) + 1);
		strcpy(shaderPath, path);
	}
	
	int create(const char* vertexShader, const char* fragmentShader)
	{
		char* vsPath = (char *)malloc(sizeof(char) *
									  (strlen(shaderPath) + strlen(vertexShader)) + 1);
		strcpy(vsPath, shaderPath);
		strcat(vsPath, vertexShader);
		char* fsPath = (char *)malloc(sizeof(char) *
									  (strlen(shaderPath) + strlen(fragmentShader)) + 1);
		strcpy(fsPath, shaderPath);
		strcat(fsPath, fragmentShader);
		
		GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

		char* vertSource = Utils::loadFileIntoCString(vsPath);
		char* fragSource = Utils::loadFileIntoCString(fsPath);

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
		glBindAttribLocation(program, 3, "vColor");

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
			shaderList[index] = newObject;
		}
		else
		{
			shaderList.push_back(newObject);
			index = shaderList.size() - 1;
		}

		Log::message(std::string(vertexShader) + ", " + std::string(fragmentShader) +
					 " compiled into shader program");
		free(vsPath);
		free(fsPath);
		
		return index;
	}

	void bindShader(const unsigned int shaderIndex)
	{
		ShaderObject shaderObject = shaderList[shaderIndex];
		glUseProgram(shaderObject.program);
	}

	void unbindActiveShader()
	{
		glUseProgram(0);
	}

	GLint getUniformLocation(const unsigned int shaderIndex, const char* name)
	{
		ShaderObject shaderObject = shaderList[shaderIndex];
		GLint handle = glGetUniformLocation(shaderObject.program, name);

		if(handle == -1)
			Log::error("Shader::getUniformLocation", "Invalid uniform " + std::string(name));

		return handle;
	}

	void setUniformInt(const unsigned int shaderIndex, const char* name, const int value)
	{
		GLint location = getUniformLocation(shaderIndex, name);
		if(location >= 0)
			glUniform1i(location, value);
	}
	
	void setUniformFloat(const unsigned int shaderIndex, const char* name, const float value)
    {
		GLint location = getUniformLocation(shaderIndex, name);
		if(location >= 0)
			glUniform1f(location, value);
	}
	
	void setUniformVec2(const unsigned int shaderIndex,  const char* name, const Vec2 value)
	{
		GLint location = getUniformLocation(shaderIndex, name);
		if(location >= 0)
			glUniform2fv(location, 1, glm::value_ptr(value));
	}
	
	void setUniformVec3(const unsigned int shaderIndex,  const char* name, const Vec3 value)
	{
		GLint location = getUniformLocation(shaderIndex, name);
		if(location >= 0)
			glUniform3fv(location, 1, glm::value_ptr(value));
	}
	
	void setUniformVec4(const unsigned int shaderIndex,  const char* name, const Vec4 value)
	{
		GLint location = getUniformLocation(shaderIndex, name);
		if(location >= 0)
			glUniform4fv(location, 1, glm::value_ptr(value));
	}
	
	void setUniformMat4(const unsigned int shaderIndex,  const char* name, const Mat4 value)	
	{
		GLint location = getUniformLocation(shaderIndex, name);
		if(location >= 0)
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void remove(const unsigned int shaderIndex)
	{
		ShaderObject shaderObject = shaderList[shaderIndex];
		glDeleteProgram(shaderObject.program);
		glDeleteShader(shaderObject.vertexShader);
		glDeleteShader(shaderObject.fragmentShader);
		emptyIndices.push_back(shaderIndex);
	}
	
	void cleanup()
	{
		free(shaderPath);
		for(unsigned int i = 0; i < shaderList.size(); i++)
			remove(i);

		shaderList.clear();
		emptyIndices.clear();
	}
}
