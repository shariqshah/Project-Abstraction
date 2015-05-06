#include <GL/gl.h>
#include <vector>
#include <string.h>

#include "../include/SDL2/SDL_image.h"
#include "../include/SDL2/SDL_surface.h"
#include "../include/SDL2/SDL_main.h"

#include "texture.h"
#include "log.h"
#include "renderer.h"
#include "scriptengine.h"
#include "passert.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

namespace Texture
{
	unsigned int createTexture(int target,
							   int width,
							   int height,
							   int format,
							   int internalFormat,
							   int type,
							   void* data,
							   int levels = 1);
	int    createNewIndex();
	
	struct TextureObj
	{
		SDL_Surface*  surface;
		unsigned int  id;
		char*         name;
		uint32_t      refCount = 0;
		int           target   = 0;
	};
	
	namespace
	{	
		std::vector<TextureObj> textureList;
		std::vector<int>        emptyIndices;
		char*                   texturePath;
		int                     currentTarget = 0;

		int isLoaded(const char* name)
		{
			int loaded = -1;
			for(uint i = 0; i < textureList.size(); i++)
			{
				if(!textureList[i].name)
				{
					continue;
				}
				else if(strcmp(name, textureList[i].name) == 0)
				{
					loaded = i;
					break;
				}
			}
			return loaded;
		}
	}

	int createNewIndex()
	{
		int index = -1;
		if(!emptyIndices.empty())
		{
			index = emptyIndices.back();
			emptyIndices.pop_back();
		}
		else
		{
			textureList.push_back(TextureObj());
			index = textureList.size() - 1;
		}
		return index;
	}

	unsigned int createTexture(int target,
							   int width,
							   int height,
							   int format,
							   int internalFormat,
							   int type,
							   void* data,
							   int levels)
	{
		GLuint id = 0;
		glGenTextures(1, &id);
		glBindTexture(target, id);
		if(target == GL_TEXTURE_2D)
		{
			glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data);
		}
		else if(target == GL_TEXTURE_2D_ARRAY)
		{
			for(int i = 0; i < levels; i++)
				glTexImage3D(target, i, internalFormat, width, height, levels, 0, format, type, data);
		}
		Renderer::checkGLError("Texture::createTexture");
		glBindTexture(target, 0);
		return id;
	}

	void setTextureParameter(int index, int parameter, int value)
	{
		TextureObj* textureObj = NULL;
		if(index > -1 && index < (int)textureList.size())
			textureObj = &textureList[index];
		else
			return;

		GLint currentTexture = 0;
		int bindingTarget = textureObj->target == GL_TEXTURE_2D ?
			GL_TEXTURE_BINDING_2D : GL_TEXTURE_BINDING_2D_ARRAY;
		glGetIntegerv(bindingTarget, &currentTexture);
		glBindTexture(textureObj->target, textureObj->id);
		glTexParameteri(textureObj->target, parameter, value);
		Renderer::checkGLError("Texture::setTextureParameter");
		if(currentTexture != 0)
			glBindTexture(textureObj->target, currentTexture);
	}

	void initialize(const char* path)
	{
		texturePath   = (char *)malloc(sizeof(char) * strlen(path) + 1);
		strcpy(texturePath, path);
		// int flags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
		int flags = IMG_INIT_PNG | IMG_INIT_TIF;
		int success = IMG_Init(flags);
		PA_ASSERT(flags == success);
	}
	
	int create(const char* filename)
	{
		int index = isLoaded(filename);
		if(index == -1)
		{
			char* fullPath = (char *)malloc(sizeof(char) *
										(strlen(texturePath) + strlen(filename)) + 1);
			strcpy(fullPath, texturePath);
			strcat(fullPath, filename);
		
			SDL_Surface* newSurface = IMG_Load(fullPath);
			free(fullPath);

			if(newSurface)
			{
				int format         = GL_RGB;
				int internalFormat = GL_RGB;
				int textureType    = GL_UNSIGNED_BYTE;
				if (newSurface->format->BytesPerPixel == 4)
				{
					if(newSurface->format->Rmask == 0xff)
					{
						format = GL_RGBA;
						textureType = GL_UNSIGNED_INT_8_8_8_8_REV;
					}
					else
					{
						format = GL_BGRA;
						textureType = GL_UNSIGNED_INT_8_8_8_8;
					}
					internalFormat = GL_RGBA8;
				}
				else if(newSurface->format->BytesPerPixel == 3)
				{
					if(newSurface->format->Rmask == 0xff)
						format = GL_RGB;
					else
						format = GL_BGR;
					internalFormat = GL_RGB8;
				}

				SDL_LockSurface(newSurface);
				GLuint id = createTexture(GL_TEXTURE_2D,
										  newSurface->w,
										  newSurface->h,
										  format,
										  internalFormat,
										  textureType,
					                      newSurface->pixels);
				SDL_UnlockSurface(newSurface);

				index = createNewIndex();
				TextureObj *newTexture = &textureList[index];
				newTexture->id         = id;
				newTexture->surface    = newSurface;
				newTexture->target     = GL_TEXTURE_2D;
				newTexture->refCount++;
				setTextureParameter(index, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				setTextureParameter(index, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				setTextureParameter(index,GL_TEXTURE_WRAP_S,GL_REPEAT);
				setTextureParameter(index,GL_TEXTURE_WRAP_T,GL_REPEAT);
			
				if(newTexture->name != NULL)
					free(newTexture->name);
		
				newTexture->name = (char *)malloc(strlen(filename) + 1);
				strcpy(newTexture->name, filename);
				Log::message("Texture : " + std::string(filename) + " created.");
			}
			else
			{
				Log::error("Texture::create", IMG_GetError());
			}
		}
		else
		{
			textureList[index].refCount++;
		}
		return index;
	}

	int create(const char* name,
			   int target,
			   int width,
			   int height,
			   int format,
			   int internalFormat,
			   int type,
			   void* data,
			   int levels)
	{
		GLuint id = createTexture(target,
								  width,
								  height,
								  format,
								  internalFormat,
								  type,
								  data,
								  levels);
		int index = createNewIndex();
		TextureObj *newTexture = &textureList[index];
		newTexture->id         = id;
		newTexture->surface    = NULL;
		newTexture->target     = target;
		newTexture->name       = (char*)malloc(strlen(name) + 1);
		strcpy(newTexture->name, name);
		newTexture->refCount++;
		Log::message("New custom texture created");
		return index;
	}
	
	void remove(int textureIndex)
	{
		if(textureIndex >=0 && textureIndex < (int)textureList.size())
		{
			TextureObj *textureObj = &textureList[textureIndex];
			if(textureObj->refCount == 1)
			{
				glDeleteTextures(1, &textureObj->id);
				textureObj->id = 0;
				if(textureObj->surface != NULL)
				{
					// free(textureObj->surface->pixels);
					SDL_FreeSurface(textureObj->surface);
					textureObj->surface = NULL;
				}
				if(textureObj->name != NULL)
				{
					free(textureObj->name);
					textureObj->name = NULL;
				}
				emptyIndices.push_back(textureIndex);
			}
			textureObj->refCount--;
		}
		else
		{
			Log::error("Texture::remove", "Texture index " + std::to_string(textureIndex) + " is invalid");
		}
	}
	
	void bind(int textureIndex)
	{
		if(textureIndex >= 0 && textureIndex < (int)textureList.size())
		{
			TextureObj* obj = &textureList[textureIndex];
			glBindTexture(obj->target, obj->id);
			//glBindTexture(GL_TEXTURE_2D, obj->id);
			currentTarget = obj->target;
			Renderer::checkGLError("Texture::bind");
		}
		else
		{
			Log::error("Texture::bind", "Texture index " + std::to_string(textureIndex) + " is invalid");
		}
	}

	const char* getFilename(int textureIndex)
	{		
		if(textureIndex >= 0 && textureIndex < (int)textureList.size())
		{
			TextureObj* obj = &textureList[textureIndex];
			return obj->name;
		}
		else
		{
			Log::error("Texture::getFilename",
					   "Texture index " + std::to_string(textureIndex) + "is invalid");
			return NULL;
		}
	}

	void unbind()
	{
		glBindTexture(currentTarget, 0);
		currentTarget = 0;
	}

	void increaseRefCount(int textureIndex)
	{
		if(textureIndex >= 0 && textureIndex < (int)textureList.size())
		{
			textureList[textureIndex].refCount++;
		}
		else
		{
			Log::error("Texture::increaseRefcount",
					   "Texture index " + std::to_string(textureIndex) + "is invalid");
		}
	}

	void decreaseRefCount(int textureIndex)
	{
		if(textureIndex >= 0 && textureIndex < (int)textureList.size())
		{
			if(--textureList[textureIndex].refCount == 0)
				remove(textureIndex);
		}
		else
		{
			Log::error("Texture::decreaseRefcount",
					   "Texture index " + std::to_string(textureIndex) + "is invalid");
		}
	}

	void cleanup()
	{
		free(texturePath);
		for(unsigned int i = 0; i < textureList.size(); i ++)
			remove(i);
		textureList.clear();
		emptyIndices.clear();
		IMG_Quit();
	}

	void generateBindings()
	{
		asIScriptEngine* engine = ScriptEngine::getEngine();
		int rc = engine->SetDefaultNamespace("Texture");
		rc = engine->RegisterGlobalFunction("int create(const string)",
											asFUNCTIONPR(create, (const char*), int),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->RegisterGlobalFunction("void remove(string)", asFUNCTION(remove), asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->SetDefaultNamespace("");
	}

	unsigned int getTextureID(int textureIndex)
	{
		unsigned int id = 0;
		if(textureIndex > -1 && textureIndex < (int)textureList.size())
			id = textureList[textureIndex].id;
		return id;
	}
}
