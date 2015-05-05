#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>

#include "framebuffer.h"
#include "texture.h"
#include "log.h"
#include "renderer.h"

namespace Framebuffer
{
	struct FBO
	{
		GLuint fbo;
		GLuint renderbuffer;
		int    texture;
		int    width;
		int    height;
	};

	namespace
	{
		std::vector<FBO> framebufferList;
		std::vector<int> emptyIndices;
	}
	
	int create(int width, int height, bool hasDepthAttachment, bool hasColorAttachment)
	{
		int index   = -1;
		GLuint fbo;
		GLuint renderbuffer;
		
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		if(hasColorAttachment)
		{
			glGenRenderbuffers(1, &renderbuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
			glRenderbufferStorage(GL_RENDERBUFFER,
								  GL_DEPTH24_STENCIL8,
								  width,
								  height);
			Renderer::checkGLError("Framebuffer::create");
			glFramebufferRenderbuffer(GL_FRAMEBUFFER,
									  GL_DEPTH_STENCIL_ATTACHMENT,
									  GL_RENDERBUFFER,
									  renderbuffer);
			Renderer::checkGLError("Framebuffer::create");
		}
		else
		{
			glDrawBuffer(GL_NONE);
		}
		Renderer::checkGLError("Framebuffer::create");
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if(status != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::error("Framebuffer::create", "Framebuffer not created!");
			Renderer::checkGLError("Framebuffer::create");
		}
		else
		{
			if(emptyIndices.empty())
			{
				framebufferList.push_back(FBO());
				index = (int)framebufferList.size() - 1;
			}
			else
			{
				index = emptyIndices.back();
				emptyIndices.pop_back();
			}
			FBO* framebuffer = &framebufferList[index];
			framebuffer->fbo          = fbo;
			framebuffer->renderbuffer = renderbuffer;
			framebuffer->texture      = -1;
			framebuffer->width        = width;
			framebuffer->height       = height;
			Log::message("Framebuffer created successfully");
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		return index;
	}
	
	void remove(int index)
	{
		if(index > -1 && index < (int)framebufferList.size())
		{
			FBO* framebuffer = &framebufferList[index];
			if(framebuffer->texture != -1) Texture::remove(framebuffer->texture);
			glDeleteRenderbuffers(1, &framebuffer->renderbuffer);
			glDeleteFramebuffers(1, &framebuffer->fbo);
		}
	}
	
	void bind(int index)
	{
		if(index > -1 && index < (int)framebufferList.size())
		{
			FBO* framebuffer = &framebufferList[index];
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->fbo);
		}
	}
	
	void unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void cleanup()
	{
		for(int i = 0; i < (int)framebufferList.size(); i++)
			remove(i);
	}

	void bindTexture(int index)
	{
		if(index > -1 && index < (int)framebufferList.size())
		{
			FBO* framebuffer = &framebufferList[index];
			Texture::bind(framebuffer->texture);
		}
	}

	int getWidth(int index)
	{
		int width = -1;
		if(index > -1 && index < (int)framebufferList.size())
		{
			FBO* framebuffer = &framebufferList[index];
			width = framebuffer->width;
		}
		return width;
	}

	int getHeight(int index)
	{
		int height = -1;
		if(index > -1 && index < (int)framebufferList.size())
		{
			FBO* framebuffer = &framebufferList[index];
			height = framebuffer->height;
		}
		return height;
	}

	void setTexture(int index, int texture, int attachment)
	{
		if(index > -1 && index < (int)framebufferList.size())
		{
			GLint currentFBO = 0;
			glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &currentFBO);
			Renderer::checkGLError("Framebuffer::setTexture, glGet");
			bind(index);
			glFramebufferTexture2D(GL_FRAMEBUFFER,
								   attachment,
								   GL_TEXTURE_2D,
								   Texture::getTextureID(texture),
								   0);
			Renderer::checkGLError("Framebuffer::setTexture, glFramebufferTexture2D");
			glBindFramebuffer(GL_FRAMEBUFFER, currentFBO);
		}
	}

	int getTexture(int index)
	{
		int texture = -1;
		if(index > -1 && index < (int)framebufferList.size())
		{
			FBO* framebuffer = &framebufferList[index];
			texture = framebuffer->texture;
		}
		return texture;
	}
	
}
