#pragma once

#include "../../RenderImpl.h"
#include <glad/gl.h>

namespace VGF::Opengl
{
	class OpenglFrameBuffer : public FrameBufferImpl
	{
	public:
		OpenglFrameBuffer(void* framebufferImage, unsigned int width, unsigned int height);
		~OpenglFrameBuffer();

		virtual void Bind() override;
		virtual void UnBind() override;
		virtual void Delete() override;

	private:
		GLint _viewport[4];
		unsigned int _width, _height;
		GLuint _framebufferImage;
		GLuint _framebuffer;
		GLuint _renderbuffer;
	};


}