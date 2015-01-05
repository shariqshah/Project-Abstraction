#ifndef texture_H
#define texture_H

namespace Texture
{
	int  create(const char* filename);
    void remove(unsigned int textureIndex);
	void initialize(const char* path);
	void bindTexture(unsigned int textureIndex);	
	void unBindActiveTexture();
	void cleanup();
}

#endif
