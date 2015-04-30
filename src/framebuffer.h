#ifndef framebuffer_H
#define framebuffer_H

namespace Framebuffer
{
	int  create(const char* name, int width, int height, bool isDepthMap);
	void bind(int index);
	void remove(int index);
	void bindTexture(int index);
	void unbind();
	void cleanup();
	int  getWidth(int index);
	int  getHeight(int index);
}

#endif
