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

		io = &ImGui::GetIO();
	}

	Gui::~Gui()
	{
		if (impl) delete impl;
		ImGui::DestroyContext();
	}

	void Gui::NewFrame(const Window& window)
	{
		impl->NewFrame();
		ImGui::NewFrame();
	}

    void Gui::Render() {
		ImGui::Render();
		impl->Render();
    }

}