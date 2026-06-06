#pragma once

#include "Window.h"
#include "Renderer.h"
#include "RenderImpl.h"

#include <imgui/imgui.h>
#include <iostream>

namespace VGF
{

	class Gui
	{
	public:
		Gui(const Window& window);
		~Gui();

        void NewFrame();
		void Render(const Window& window);

	private:
		GuiImpl* impl;
	};
}