#pragma once

#include "../../RenderImpl.h"
#include <glad/glad.h>

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
		GLuint _framebufferImage;
	};


}