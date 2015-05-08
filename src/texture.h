#ifndef texture_H
#define texture_H

enum TextureUnit
{
	TU_ALBEDO = 0,
	TU_SHADOWMAP0,
	TU_SHADOWMAP1,
	TU_SHADOWMAP2,
	TU_SHADOWMAP3
};

namespace Texture
{
	int          create(const char* filename);
    void         remove(int textureIndex);
	void         setTextureParameter(int textureIndex, int parameter, int value);
	void         initialize(const char* path);
	void         bind(int textureIndex);
	const char*  getFilename(int textureIndex);
	void         unbind();
	void         cleanup();
	void         generateBindings();
	void         increaseRefCount(int textureIndex);
	void         decreaseRefCount(int textureIndex);
	unsigned int getTextureID(int textureIndex);
	int          create(const char* name,
						int   target,
						int   width,
						int   height,
						int   format,
						int   internalFormat,
						int   type,
						void* data,
						int   levels = 1);
}

#endif
