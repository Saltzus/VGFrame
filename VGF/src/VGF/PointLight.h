#pragma once
#include <glm/glm.hpp>
#include <set>

#include "Camera.h"
#include "Renderer.h"
#include "RenderData.h"

namespace VGF
{
	struct PointLightData
	{
		alignas(16) glm::vec3 position;
		alignas(16) glm::vec3 color;
	};

	class PointLight
	{
	public:
		PointLight(glm::vec3 position, glm::vec3 color);
		PointLight() : PointLight({0,0,0}, {1,1,1}) {};
		~PointLight();

		PointLight(const PointLight&) = delete;
		PointLight& operator=(const PointLight&) = delete;

		glm::vec3 position;
		glm::vec3 color;
		bool enabled = true;

		void Render(const Camera& camera, glm::mat4 parentMatrix = 1.f);
		static std::vector<PointLightData> GetLights();

	private:
		static inline std::vector<PointLight*> pointLights;

		static inline std::vector<float> vertices =
		{	
			-.25f, -.25f, 0.f,		0.f,0.f,
			 .25f, -.25f, 0.f,		1.f,0.f,
			 .25f,  .25f, 0.f,		1.f,1.f,
			-.25f,  .25f, 0.f,		1.f,0.f
		};

		static inline std::vector<unsigned int> indices = { 0,1,2, 2,3,0, 0,2,1, 2,0,3 };

		static const Shader& GetShader()
		{
			static const Shader shader
			(
				Resource::Get("Shaders/light_vert.slang"),
				Resource::Get("Shaders/light_frag.slang"),
				Topology::TRIANGLE_LIST,
				true
			);
			return shader;
		}

		std::unique_ptr<Renderer> model;
		RenderData data = RenderData(GetShader());
	};
}