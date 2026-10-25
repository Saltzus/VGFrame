#pragma once

#include "../../RenderImpl.h"

#include <vulkan/vulkan.hpp>

namespace VGF::Vulkan
{
	class Vulkan;
	class VulkanRenderer;

	struct RenderingData;

	class VulkanFrameBuffer : public FrameBufferImpl
	{
	public:
		VulkanFrameBuffer(void* imageView, unsigned int width, unsigned int height, bool offscreen = true);
		~VulkanFrameBuffer();

		void Recreate();

		bool GetOffscreen() { return _offscreen; }

		virtual void Bind() override;
		virtual void UnBind() override;
		virtual void Delete() override;

		std::vector<RenderingData> opaqueObjects;
		std::vector<RenderingData> translucentObjects;

		VkFramebuffer framebuffer;

		VkExtent2D extent;
		VkImageView imageView;

	private:
		bool _offscreen;
		
		Vulkan* _vulkan = nullptr;
	};
}