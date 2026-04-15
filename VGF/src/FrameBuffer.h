#pragma once

#include "RenderImpl.h"
#include "Texture.h"

namespace VGF
{
	class Texture;

	class FrameBuffer
	{
	public:
		FrameBuffer(Texture& framebufferTexture);
		~FrameBuffer();

		void Bind();
		void UnBind();
		void Delete();

	private:
		FrameBufferImpl* _impl = nullptr;
		Texture& _framebufferTexture;
	};

}
