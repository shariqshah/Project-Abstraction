#include <GL/gl.h>
#include <vector>
#include <string.h>
#include <assert.h>

#include "../include/SDL2/SDL_image.h"
#include "../include/SDL2/SDL_surface.h"
#include "../include/SDL2/SDL_main.h"

#include "texture.h"
#include "log.h"
#include "renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

namespace Texture
{
	struct TextureObj
	{
		SDL_Surface*  surface;
		unsigned int  id;
		char*         name;
	};
	namespace
	{	
		std::vector<TextureObj> textureList;
		std::vector<int>        emptyIndices;
		char*                   texturePath;
	}

	void initialize(const char* path)
	{
		texturePath   = (char *)malloc(sizeof(char) * strlen(path) + 1);
		strcpy(texturePath, path);

		int flags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
		int success = IMG_Init(flags);

		assert(flags == success);
	}
	
	int create(const char* filename)
	{
		char* fullPath = (char *)malloc(sizeof(char) *
										(strlen(texturePath) + strlen(filename)) + 1);
		strcpy(fullPath, texturePath);
		strcat(fullPath, filename);
		
		SDL_Surface* newSurface = IMG_Load(fullPath);
		// SDL_Surface* newSurface = SDL_LoadBMP(fullPath);
		// int w, h, n;
		// unsigned char *data = stbi_load(fullPath, &w, &h, &n, 0);
		free(fullPath);

		if(!newSurface)
			Log::error("Texture::create", IMG_GetError());
		
		assert(newSurface != NULL);
		
		GLuint id = 0;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		int format = GL_RGB;
		// if(newSurface->format->BytesPerPixel == 4)
		// 	format = GL_RGBA;

		if (newSurface->format->BytesPerPixel == 4)
        {
            if (newSurface->format->Rmask == 0x000000ff)
				format = GL_RGBA;
            else
				format = GL_BGRA;
        }
        else if (newSurface->format->BytesPerPixel == 3)
        {
            if (newSurface->format->Bmask == 0xff)
				format = GL_BGR;
            else
				format = GL_RGB;
        }

		SDL_LockSurface(newSurface);

		
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 format,
					 newSurface->w,
					 newSurface->h,
					 0,
					 format,
					 GL_UNSIGNED_BYTE,
					 newSurface->pixels);

		// glTexImage2D(GL_TEXTURE_2D,
		// 			 0,
		// 			 format,
		// 			 w,
		// 			 h,
		// 			 0,
		// 			 format,
		// 			 GL_UNSIGNED_BYTE,
		// 			 data);

		Renderer::checkGLError("Texture::create");
		SDL_UnlockSurface(newSurface);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);

		// free(data);
		
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

		TextureObj *newTexture = &textureList[index];
		newTexture->id = id;
		newTexture->surface = newSurface;
		
		if(newTexture->name != NULL)
			free(newTexture->name);
		
		newTexture->name = (char *)malloc(strlen(filename) + 1);
		strcpy(newTexture->name, filename);

		Log::message("Texture : " + std::string(filename) + " created.");
		return index;
	}
	
	void remove(unsigned int textureIndex)
	{
		TextureObj *textureObj = &textureList[textureIndex];
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

	void bindTexture(unsigned int textureIndex)
	{
		TextureObj obj = textureList[textureIndex];
		glBindTexture(GL_TEXTURE_2D, obj.id);
		Renderer::checkGLError("Texture::bind");
	}

	void unBindActiveTexture()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
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
}
