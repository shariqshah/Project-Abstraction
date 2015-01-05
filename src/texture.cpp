#include <GL/gl.h>
#include <vector>
#include <string.h>
#include <assert.h>

#include "../include/SDL2/SDL_image.h"
#include "../include/SDL2/SDL_surface.h"

#include "texture.h"
#include "log.h"

namespace Texture
{
	namespace
	{
		struct TextureObj
		{
			SDL_Surface*  surface;
			unsigned int  id;
			char*         name;
		};
		
		std::vector<TextureObj> textureList;
		std::vector<int>        emptyIndices;
		char*                   texturePath;
	}

	void initialize(const char* path)
	{
		texturePath   = (char *)malloc(sizeof(char) * strlen(path) + 1);
		strcpy(texturePath, path);

		int flags = IMG_INIT_JPG | IMG_INIT_PNG;
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
		free(fullPath);
		assert(newSurface != NULL);

		GLuint id = 0;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		int format = GL_RGB;
		if(newSurface->format->BytesPerPixel == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 format,
					 newSurface->w,
					 newSurface->h,
					 0,
					 format,
					 GL_UNSIGNED_BYTE,
					 newSurface->pixels);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);
		
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

		TextureObj newTexture = textureList[index];
		newTexture.id = id;
		newTexture.surface = newSurface;
		
		if(newTexture.name != NULL)
			free(newTexture.name);
		
		newTexture.name = (char *)malloc(strlen(filename) + 1);
		strcpy(newTexture.name, filename);

		Log::message("Texture : " + std::string(filename) + " created.");
		return index;
	}
	
	void remove(unsigned int textureIndex)
	{
		TextureObj textureObj = textureList[textureIndex];
		glDeleteTextures(1, &textureObj.id);
		textureObj.id = 0;
		if(textureObj.surface != NULL)
		{
			free(textureObj.surface->pixels);
			SDL_FreeSurface(textureObj.surface);
			textureObj.surface = NULL;
		}

		if(textureObj.name != NULL)
		{
			free(textureObj.name);
			textureObj.name = NULL;
		}

		emptyIndices.push_back(textureIndex);
	}

	void bindTexture(unsigned int textureIndex)
	{
		TextureObj obj = textureList[textureIndex];
		glBindTexture(GL_TEXTURE_2D, obj.id);
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
