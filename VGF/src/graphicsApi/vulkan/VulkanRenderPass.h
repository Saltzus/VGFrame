#pragma once

#include "../../RenderImpl.h"
#include "VulkanManager.h"

namespace VGF::Vulkan
{
	class VulkanRenderPass
	{
	public:
		VulkanRenderPass();
		~VulkanRenderPass();

	private:
		VkRenderPass render;
	};
}