#ifndef texture_H
#define texture_H

namespace Texture
{
	int          create(const char* filename);
	int          create(int width, int height, int format, int internalFormat, int type, void* data);
    void         remove(int textureIndex);
	void         setTextureParameter(int textureIndex, int parameter, int value);
	void         initialize(const char* path);
	void         bindTexture(int textureIndex);
	const char*  getFilename(int textureIndex);
	void         unbindActiveTexture();
	void         cleanup();
	void         generateBindings();
	void         increaseRefCount(int textureIndex);
	void         decreaseRefCount(int textureIndex);
	unsigned int getTextureID(int textureIndex);
}

#endif
