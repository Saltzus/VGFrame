#include "DirectionalLight.h"

namespace VGF
{
	DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec3 color) : direction(direction), color(color)
	{
		directionalLights.push_back(*this);
	}

	DirectionalLight::~DirectionalLight()
	{
	}

	std::vector<DirectionalLight> DirectionalLight::GetLights()
	{
		return directionalLights;
	}
}