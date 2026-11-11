#include "Skybox.h"

namespace VGF
{
	Skybox::Skybox(std::vector<std::string> skyBoxTextures) 
	{
		model = std::make_unique<Renderer>(GetShader(), indices, vertices);
		cubemap = std::make_unique<Texture>(skyBoxTextures);
	}

	Skybox::~Skybox() = default;

	void Skybox::Render(const Camera& camera)
	{
		data.Set("MatrixBuffer.proj", camera.projection);
		data.Set("MatrixBuffer.view", camera.view);
		data.Set("samplerCube", (const Texture*)cubemap.get());

		model->Render(data, GetShader());
	}
}
