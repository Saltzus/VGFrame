#include "OpenglFrameBuffer.h"

#include "Log.h"

namespace VGF::Opengl
{
	OpenglFrameBuffer::OpenglFrameBuffer(void* framebufferImage, unsigned int width, unsigned int height) : _width(width), _height(height)
	{
		_framebufferImage = static_cast<GLuint>(reinterpret_cast<uintptr_t>(framebufferImage));

		glGenFramebuffers(1, &_framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _framebufferImage, 0);

		glGenRenderbuffers(1, &_renderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, _renderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _renderbuffer);

		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			VGF::Log::Error("Framebuffer is not complete!\n");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	OpenglFrameBuffer::~OpenglFrameBuffer()
	{
	}

	void OpenglFrameBuffer::Bind()
	{
		glGetIntegerv(GL_VIEWPORT, _viewport);
		glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, _width, _height);
	}
	void OpenglFrameBuffer::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, _viewport[2], _viewport[3]);
	}
	void OpenglFrameBuffer::Delete() {}
}
