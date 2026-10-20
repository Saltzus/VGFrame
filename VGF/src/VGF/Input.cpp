#include "Input.h"
#include <glm/glm.hpp>

#include "Gui.h"
#include "Physics.h"

namespace VGF::Input
{
	void ScreenPosToWorldRay
	(
		int mouseX, int mouseY,
		int screenWidth, int screenHeight,
		glm::mat4 ViewMatrix,
		glm::mat4 ProjectionMatrix,
		glm::vec3& out_origin,
		glm::vec3& out_direction
	)
	{
		glm::vec4 lRayStart_NDC
		(
			((float)mouseX / (float)screenWidth - 0.5f) * 2.0f,
			((float)mouseY / (float)screenHeight - 0.5f) * 2.0f,
			-1.0,
			1.0f
		);
		glm::vec4 lRayEnd_NDC
		(
			((float)mouseX / (float)screenWidth - 0.5f) * 2.0f,
			((float)mouseY / (float)screenHeight - 0.5f) * 2.0f,
			0.0,
			1.0f
		);


		glm::mat4 InverseProjectionMatrix = glm::inverse(ProjectionMatrix);
		glm::mat4 InverseViewMatrix = glm::inverse(ViewMatrix);

		glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera /= lRayStart_camera.w;
		glm::vec4 lRayStart_world = InverseViewMatrix * lRayStart_camera; lRayStart_world /= lRayStart_world.w;
		glm::vec4 lRayEnd_camera = InverseProjectionMatrix * lRayEnd_NDC;      lRayEnd_camera /= lRayEnd_camera.w;
		glm::vec4 lRayEnd_world = InverseViewMatrix * lRayEnd_camera;   lRayEnd_world /= lRayEnd_world.w;

		glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
		lRayDir_world = glm::normalize(lRayDir_world);

		out_origin = glm::vec3(lRayStart_world);
		out_direction = glm::normalize(lRayDir_world);
	}

	bool f1Pressed = false;
    void processInput(GLFWwindow* window)
    {
		if (Gui::io && Gui::io->WantCaptureKeyboard) return;


		if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
		{
			if (!f1Pressed)
			{
				Physics::GetInstance()->debugRenderOn = !Physics::GetInstance()->debugRenderOn;
				f1Pressed = true;
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_RELEASE)
		{
			f1Pressed = false;
		}
    }

	std::map<int,bool> isPressed;
	bool pressedKey(GLFWwindow* window, int key)
	{
		if (Gui::io && Gui::io->WantCaptureKeyboard) return false;
		if (glfwGetKey(window, key) == GLFW_PRESS)
		{
			if (!isPressed[key])
			{
				isPressed[key] = true;
				return true;
			}
		}
		else if (glfwGetKey(window, key) == GLFW_RELEASE)
		{
			isPressed[key] = false;
			return false;
		}

		return false;
	}

	bool pressingKey(GLFWwindow* window, int key)
	{
		if (Gui::io && Gui::io->WantCaptureKeyboard) return false;
		return glfwGetKey(window, key) == GLFW_PRESS;
	}

	std::map<int, bool> isPressedMouse;
	bool pressedMouseButton(GLFWwindow* window, int button)
	{
		if (Gui::io && Gui::io->WantCaptureMouse) return false;
		if (glfwGetMouseButton(window, button) == GLFW_PRESS)
		{
			if (!isPressedMouse[button])
			{
				isPressedMouse[button] = true;
				return true;
			}
		}
		else if (glfwGetMouseButton(window, button) == GLFW_RELEASE)
		{
			isPressedMouse[button] = false;
			return false;
		}

		return false;
	}

	bool pressingMouseButton(GLFWwindow* window, int button)
	{
		if (Gui::io && Gui::io->WantCaptureMouse) return false;
		return glfwGetMouseButton(window, button) == GLFW_PRESS;
	}

	std::pair<double, double> GetMousePosition(GLFWwindow* window)
	{
		double posX, posY;
		int width, height;

		GetMousePosition(window, posX, posY);
		glfwGetWindowSize(window, &width, &height);

		//posX += width;
		posY += height;

		return std::make_pair(posX, posY);
	}

	void GetMousePosition(GLFWwindow* window, double& posX, double& posY)
	{
		glfwGetCursorPos(window, &posX, &posY);
		posY = -posY;
	}

	bool pickObject(const Window* window, Physics& physics, Camera* camera, b3BodyId& outBodyID, glm::vec3& outHitPos, const b3BodyId& ignoreBody)
	{
		glm::vec3 origin;
		glm::vec3 direction;

		auto [mouseX, mouseY] = GetMousePosition(window->GLFW_Window);
		ScreenPosToWorldRay((int)mouseX, (int)mouseY, window->width, window->height, camera->view, camera->projection, origin, direction);

		float rayDistance = 1000.0f;
		b3Vec3 rayOrigin = { origin.x, origin.y , origin.z };
		b3Vec3 rayDirection = { direction.x, direction.y , direction.z };

		b3QueryFilter queryFilter = b3DefaultQueryFilter();

		RaycastContext context;
		context.ignoreBody = ignoreBody;

		b3World_CastRay(physics.worldId, rayOrigin, rayDirection, queryFilter, RayCastCallback, &context);

		if (context.hit) return true;
		else return false;
	}
}
