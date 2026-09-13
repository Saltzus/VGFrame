#pragma once
#include "../../RenderImpl.h"

namespace VGF::Opengl
{
	class OpenglGui : public GuiImpl
	{
	public:
		OpenglGui(const Window& window);
		~OpenglGui();

		void NewFrame() override;
		void Render() override;
	private:

	};


}