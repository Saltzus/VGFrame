#pragma once

#include "Window.h"
#include "Camera.h"

#include "PhysicsObject.h"

namespace VGF::Input
{
    void processInput(GLFWwindow* window);
	PhysicsObject* pickObject(Window* window, btDiscreteDynamicsWorld* dynamicsWorld, Camera* camera);

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

}

