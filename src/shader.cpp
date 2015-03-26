#include <GL/glew.h>
#include <GL/gl.h>
#include <string.h>
#include <vector>

#include "shader.h"
#include "utilities.h"
#include "log.h"
#include "renderer.h"
#include "passert.h"

namespace Shader
{
	struct ShaderObject
	{
		unsigned int vertexShader;
		unsigned int fragmentShader;
		unsigned int program; 
	};
	
	namespace
	{
		std::vector<ShaderObject> shaderList;
		std::vector<unsigned int> emptyIndices;
		char*                     shaderPath;
	}

	void debugPrintShader(const char* shaderText)
	{
		size_t len = strlen(shaderText);
		int lineCount = 1;
		std::cout<<std::to_string(lineCount) + ". ";
		for(uint i = 0; i < len; i++)
		{
			if(shaderText[i] != '\n')
				std::cout<<shaderText[i];
			else
				std::cout<<std::endl<<std::to_string(++lineCount) + ". ";
		}
		std::cout<<"\n END_DEBUG_PRINT\n\n";
	}

	char* runPreProcessor(char* shaderText)
	{
		char* includeLoc = strstr(shaderText, "//include");
		if(includeLoc)
		{
			// size_t len = strlen(includeLoc);
			char* lineEnd = strchr(includeLoc, '\n');
			int lineSize = lineEnd - includeLoc;
			char* incLine = (char*)malloc((sizeof(char) * lineSize) + 1);
			strncpy(incLine, includeLoc, lineSize);
			incLine[lineSize] = '\0';

			char* filename = strtok(incLine, " ");
			while(filename)
			{
				filename = strtok(NULL, " ");
				if(filename)
				{
					char* path = (char*)malloc(sizeof(char) *
											   (strlen(shaderPath) + strlen(filename) + 1));
					strcpy(path, shaderPath);
					strcat(path, filename);
					char* file = Utils::loadFileIntoCString(path);
					char* shaderCopy = strdup(shaderText);
					char* temp = (char*)realloc(shaderText, (strlen(shaderText) + strlen(file) + 2));
					if(temp)
					{
						shaderText = temp;
						strcpy(shaderText, file);
						strcat(shaderText, shaderCopy);
					}
					else
					{
						Log::warning("Realloc failed in Shader::runPreProcessor");
					}

					free(path);
					free(shaderCopy);
					free(file);
				}
			}
			// Log::message(std::string(shaderText));
			free(incLine);
		}

		return shaderText;
	}
	
	void initialize(const char* path)
	{
		shaderPath   = (char *)malloc(sizeof(char) * strlen(path) + 1);
		strcpy(shaderPath, path);
	}
	
	int create(const char* vertexShaderName, const char* fragmentShaderName)
	{
		char* vsPath = (char *)malloc(sizeof(char) *
									  (strlen(shaderPath) + strlen(vertexShaderName)) + 1);
		strcpy(vsPath, shaderPath);
		strcat(vsPath, vertexShaderName);
		char* fsPath = (char *)malloc(sizeof(char) *
									  (strlen(shaderPath) + strlen(fragmentShaderName)) + 1);
		strcpy(fsPath, shaderPath);
		strcat(fsPath, fragmentShaderName);
		
		GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

		char* vertSource = Utils::loadFileIntoCString(vsPath);
		char* fragSource = Utils::loadFileIntoCString(fsPath);

		PA_ASSERT(vertSource != NULL);
		PA_ASSERT(fragSource != NULL);

		vertSource = runPreProcessor(vertSource);		
		fragSource = runPreProcessor(fragSource);
		
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

			Log::error("COMPILING VS " + std::string(vertexShaderName), std::string(message));
			debugPrintShader(vertSource);
			free(message);
		}

		if(!isFragCompiled)
		{
			GLint logSize = 0;
			glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logSize);
			char* message = (char *)malloc(sizeof(char) * logSize);
			glGetShaderInfoLog(fragShader, logSize, NULL, message);

			Log::error("COMPILING FS " + std::string(fragmentShaderName), std::string(message));
			debugPrintShader(fragSource);
			free(message);
		}

		free(vertSource);
		free(fragSource);
		
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
		glBindAttribLocation(program, POSITION_LOC, "vPosition");
		glBindAttribLocation(program, NORMAL_LOC,   "vNormal");
		glBindAttribLocation(program, UV_LOC,       "vUV");
		glBindAttribLocation(program, COLOR_LOC,    "vColor");
		Renderer::checkGLError("Shader::create");
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
		Log::message(std::string(vertexShaderName) + ", " + std::string(fragmentShaderName) +
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

	int getUniformLocation(const unsigned int shaderIndex, const char* name)
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
