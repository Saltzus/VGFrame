#include "Input.h"
#include <glm/glm.hpp>

#include <VGF/src/Gui.h>
#include <VGF/src/Physics.h>

#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

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
		glm::vec4 lRayStart_NDC(
			((float)mouseX / (float)screenWidth - 0.5f) * 2.0f,
			((float)mouseY / (float)screenHeight - 0.5f) * 2.0f,
			-1.0,
			1.0f
		);
		glm::vec4 lRayEnd_NDC(
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
		if (Gui::io->WantCaptureKeyboard) return;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);


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
		if (Gui::io->WantCaptureKeyboard) return false;
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

	std::map<int, bool> isPressedMouse;
	bool pressedMouseButton(GLFWwindow* window, int button)
	{
		if (Gui::io->WantCaptureMouse) return false;
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

	bool pickObject(const Window* window, Physics& physics, Camera* camera, JPH::BodyID& outBodyID, glm::vec3& outHitPos)
	{
		glm::vec3 origin;
		glm::vec3 direction;

		auto [mouseX, mouseY] = GetMousePosition(window->GLFW_Window);
		ScreenPosToWorldRay((int)mouseX, (int)mouseY, window->width, window->height, camera->view, camera->projection, origin, direction);

		float rayDistance = 1000.0f;
		JPH::Vec3 rayOrigin = { origin.x, origin.y , origin.z };
		JPH::Vec3 rayDirection = { direction.x, direction.y , direction.z };
		JPH::Vec3 rayEnd = rayOrigin + rayDirection * rayDistance;


		JPH::RRayCast raycast{ rayOrigin, rayDirection * rayDistance };

		JPH::RayCastResult result;
		bool had_hit = physics.physicsSystem.GetNarrowPhaseQuery().CastRay(raycast, result);

		JPH::Vec3 outPosition = raycast.GetPointOnRay(result.mFraction);
		outHitPos = { outPosition.GetX(), outPosition.GetY(), outPosition.GetZ() };

		float outFraction = result.mFraction;
		outBodyID = result.mBodyID;

#ifdef JPH_DEBUG_RENDERER
		if (had_hit)
			physics.debugRenderer->DrawMarker(outPosition, JPH::Color::sRed, 0.1f);
		else
			physics.debugRenderer->DrawMarker(rayOrigin + 0.1f * rayDirection, JPH::Color::sYellow, 0.001f);
#endif

		return had_hit;
	}
}
