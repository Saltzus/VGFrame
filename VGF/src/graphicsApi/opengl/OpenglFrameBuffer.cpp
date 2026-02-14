#include "OpenglFrameBuffer.h"

namespace VGF::Opengl
{
	OpenglFrameBuffer::OpenglFrameBuffer(void* framebufferImage, unsigned int width, unsigned int height)
	{
		_framebufferImage = *reinterpret_cast<GLuint*>(framebufferImage);
	}

	OpenglFrameBuffer::~OpenglFrameBuffer()
	{
	}

	void OpenglFrameBuffer::Bind() {}
	void OpenglFrameBuffer::UnBind() {}
	void OpenglFrameBuffer::Delete() {}
}