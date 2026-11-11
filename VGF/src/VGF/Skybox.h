#pragma once
#include <glm/glm.hpp>
#include <set>

#include "Camera.h"
#include "Renderer.h"
#include "RenderData.h"

namespace VGF
{
	class Skybox
	{
	public:
		Skybox(std::vector<std::string> skyBoxTextures);
		~Skybox();

		void Render(const Camera& camera);
	private:
		static inline std::vector<float> vertices =
		{
			-1.0f,	 1.0f,	-1.0f,
			-1.0f,	-1.0f,	-1.0f,
			 1.0f,	-1.0f,	-1.0f,
			 1.0f,	 1.0f,	-1.0f,
			-1.0f,	 1.0f,	 1.0f,
			-1.0f,	-1.0f,	 1.0f,
			 1.0f,	-1.0f,	 1.0f,
			 1.0f,	 1.0f,	 1.0f 
		};

		static inline std::vector<unsigned int> indices = 
		{ 
			0, 1, 2,  0, 2, 3,
			4, 6, 5,  4, 7, 6,
			4, 5, 1,  4, 1, 0,
			3, 2, 6,  3, 6, 7,
			4, 0, 3,  4, 3, 7,
			1, 5, 6,  1, 6, 2
		};

		static const Shader& GetShader()
		{
			static const Shader shader
			(
				Resource::Get("Shaders/skybox_vert.slang"),
				Resource::Get("Shaders/skybox_frag.slang"),
				Topology::TRIANGLE_LIST,
				false
			);
			return shader;
		}

		std::unique_ptr<Renderer> model;
		std::unique_ptr<Texture> cubemap;

		RenderData data = RenderData(GetShader());
	};
}