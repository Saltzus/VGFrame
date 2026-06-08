#pragma once

#include "Window.h"
#include "Camera.h"

#include "PhysicsObject.h"

namespace VGF::Input
{
    void processInput(GLFWwindow* window);
	bool pressedKey(GLFWwindow* window, int key);
	bool pressedMouseButton(GLFWwindow* window, int button);

	std::pair<double,double> GetMousePosition(GLFWwindow* window);
	void GetMousePosition(GLFWwindow* window, double& posX, double& posY);

	void ScreenPosToWorldRay
	(
		int mouseX, int mouseY,
		int screenWidth, int screenHeight,
		glm::mat4 ViewMatrix,
		glm::mat4 ProjectionMatrix,
		glm::vec3& out_origin,
		glm::vec3& out_direction
	);

	bool getDebugDrawerOn();


	bool pickObject(const Window* window, Physics& physics, Camera* camera, JPH::BodyID& outBodyID, glm::vec3& outHitPos);

}

