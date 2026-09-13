#pragma once

#include <vulkan/vulkan.hpp>

#include "../../RenderImpl.h"

namespace VGF {
	class Window;
}

namespace VGF::Vulkan
{
	class VulkanGui : public GuiImpl
	{
	public:
		VulkanGui(const Window& window);
		~VulkanGui();

		static void Render(VkCommandBuffer commandBuffer);

		void NewFrame() override;
		void Render() override;

	private:
		VkDescriptorPool descriptorPool;
		VkRenderPass renderPass;
	};
}