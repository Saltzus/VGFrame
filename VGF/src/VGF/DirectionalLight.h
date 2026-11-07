#pragma once
#include <glm/glm.hpp>
#include <set>

namespace VGF
{
	class DirectionalLight
	{
	public:
		DirectionalLight(glm::vec3 direction, glm::vec3 color);
		~DirectionalLight();

		alignas(16)glm::vec3 direction;
		alignas(16)glm::vec3 color;

		void Remove() {}
		static std::vector<DirectionalLight> GetLights();

	private:
		static inline std::vector<DirectionalLight> directionalLights;
	};
}