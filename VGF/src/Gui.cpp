#include "Gui.h"

#include "Renderer.h"
#include "Texture.h"
#include "graphicsApi/vulkan/VulkanManager.h"
#include "graphicsApi/vulkan/VulkanGui.h"

namespace VGF
{
	Vulkan::VulkanGui* vulkanGui;

	Gui::Gui(const Window& window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		
		vulkanGui = new Vulkan::VulkanGui(window);
	}

	Gui::~Gui()
	{
		delete vulkanGui;
		ImGui::DestroyContext();
	}

	bool show_demo_window = true;

	void Gui::NewFrame()
	{
		vulkanGui->NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow(&show_demo_window);
	}

    void Gui::Render(const Window& window) {
		ImGui::Render();
    }

}