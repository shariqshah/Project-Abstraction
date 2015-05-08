#ifndef framebuffer_H
#define framebuffer_H

namespace Framebuffer
{
	//int  create(const char* name, int width, int height, bool isDepthMap);
	int  create(int width, int height, bool hasDepthAttachment, bool hasColorAttachment);
	void bind(int index);
	void remove(int index);
	void bindTexture(int index);
	void unbind();
	void cleanup();
	int  getWidth(int index);
	int  getHeight(int index);
	void setTexture(int index, int texture, int attachment);
	void setTextureLayer(int index, int texture, int attachment, int layer);
	int  getTexture(int index);
}

#endif
