#include "PointLight.h"

namespace VGF
{
	PointLight::PointLight(glm::vec3 position, glm::vec3 color) : position(position), color(color)
	{
		pointLights.push_back(this);
		model = std::make_unique<Renderer>(GetShader(), indices, vertices);
	}

	PointLight::~PointLight()
	{
		std::erase(pointLights, this);
	}

	void PointLight::Render(const Camera& camera, glm::mat4 parentMatrix)
	{
		data.Set("MatrixBuffer.model", glm::translate(parentMatrix, position));
		data.Set("MatrixBuffer.proj", camera.projection);
		data.Set("MatrixBuffer.view", camera.view);
		data.Set("LightColor.color", color);

		model->Render(data, GetShader());
	}

	std::vector<PointLightData> PointLight::GetLights()
	{
		std::vector<PointLightData> lights;
		for (PointLight* light : pointLights)
		{
			if (light->enabled) lights.push_back({light->position, light->color});
		}

		return lights;
	}
}