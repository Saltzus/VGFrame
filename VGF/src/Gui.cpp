#include "Gui.h"

#include "Renderer.h"
#include "Texture.h"

#include "graphicsApi/vulkan/VulkanGui.h"
#include "graphicsApi/opengl/OpenglGui.h"

namespace VGF
{
	Gui::Gui(const Window& window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		if (Renderer::GetGraphicsApi() == GraphicsApis::Vulkan)
			impl = new Vulkan::VulkanGui(window);
		else
			impl = new Opengl::OpenglGui(window);
	}

	Gui::~Gui()
	{
		if (impl) delete impl;
		ImGui::DestroyContext();
	}

	bool show_demo_window = true;

	void Gui::NewFrame()
	{
		impl->NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow(&show_demo_window);
	}

    void Gui::Render(const Window& window) {
		ImGui::Render();
		impl->Render();
    }

}