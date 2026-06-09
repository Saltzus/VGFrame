#pragma once

#include "../../RenderImpl.h"
#include <vulkan/vulkan.hpp>

namespace VGF::Vulkan
{
	class Vulkan;
	class VulkanRenderer;

	struct idObject;

	class VulkanFrameBuffer : public FrameBufferImpl
	{
	public:
		VulkanFrameBuffer(void* imageView, unsigned int width, unsigned int height, bool offscreen = true);
		~VulkanFrameBuffer();

		bool GetOffscreen() { return _offscreen; }

		virtual void Bind() override;
		virtual void UnBind() override;
		virtual void Delete() override;

		std::vector<VulkanRenderer*> opaqueObjects;
		std::vector<VulkanRenderer*> translucentObjects;

		VkFramebuffer framebuffer;

		VkExtent2D extent;

	private:
		bool _offscreen;
		
		VkImageView _imageView;
		Vulkan* _vulkan = nullptr;
	};
}