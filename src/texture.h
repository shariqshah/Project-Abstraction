#ifndef texture_H
#define texture_H

namespace Texture
{
	int         create(const char* filename);
    void        remove(int textureIndex);
	void        initialize(const char* path);
	void        bindTexture(int textureIndex);
	const char* getFilename(int textureIndex);
	void        unbindActiveTexture();
	void        cleanup();
	void        generateBindings();
	void        increaseRefCount(int textureIndex);
	void        decreaseRefCount(int textureIndex);
}

#endif
