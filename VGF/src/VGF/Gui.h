#pragma once

#include "Window.h"
#include "Renderer.h"
#include "RenderImpl.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <iostream>

namespace VGF
{

	class Gui
	{
	public:
		Gui(const Window& window);
		~Gui();

        void NewFrame(const Window& window);
		void Render();

		inline static ImGuiIO* io;

	private:
		GuiImpl* impl;
	};
}