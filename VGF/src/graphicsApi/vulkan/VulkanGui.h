#pragma once

#include <vulkan/vulkan.hpp>

namespace VGF {
	class Window;
}

namespace VGF::Vulkan
{
	class VulkanGui
	{
	public:
		VulkanGui(const Window& window);
		~VulkanGui();

		void NewFrame();
		void Render(VkCommandBuffer commandBuffer);

	private:
		
	};
}