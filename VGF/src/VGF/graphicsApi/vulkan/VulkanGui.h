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
		static void Destroy();

		void NewFrame() override;
		void Render() override;

	private:
		static inline VkDescriptorPool descriptorPool;
		static inline VkRenderPass renderPass;
	};
}