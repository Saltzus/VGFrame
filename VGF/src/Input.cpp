#include "Input.h"
#include <glm/glm.hpp>




namespace VGF::Input
{
	void ScreenPosToWorldRay
	(
		int mouseX, int mouseY,             // Mouse position, in pixels, from bottom-left corner of the window
		int screenWidth, int screenHeight,  // Window size, in pixels
		glm::mat4 ViewMatrix,               // Camera position and orientation
		glm::mat4 ProjectionMatrix,         // Camera parameters (ratio, field of view, near and far planes)
		glm::vec3& out_origin,              // Ouput : Origin of the ray. /!\ Starts at the near plane, so if you want the ray to start at the camera's position instead, ignore this.
		glm::vec3& out_direction            // Ouput : Direction, in world space, of the ray that goes "through" the mouse.
	)
	{

		// The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
		glm::vec4 lRayStart_NDC(
			((float)mouseX / (float)screenWidth - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
			((float)mouseY / (float)screenHeight - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
			-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
			1.0f
		);
		glm::vec4 lRayEnd_NDC(
			((float)mouseX / (float)screenWidth - 0.5f) * 2.0f,
			((float)mouseY / (float)screenHeight - 0.5f) * 2.0f,
			0.0,
			1.0f
		);


		// The Projection matrix goes from Camera Space to NDC.
		// So inverse(ProjectionMatrix) goes from NDC to Camera Space.
		glm::mat4 InverseProjectionMatrix = glm::inverse(ProjectionMatrix);

		// The View Matrix goes from World Space to Camera Space.
		// So inverse(ViewMatrix) goes from Camera Space to World Space.
		glm::mat4 InverseViewMatrix = glm::inverse(ViewMatrix);

		glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera /= lRayStart_camera.w;
		glm::vec4 lRayStart_world = InverseViewMatrix * lRayStart_camera; lRayStart_world /= lRayStart_world.w;
		glm::vec4 lRayEnd_camera = InverseProjectionMatrix * lRayEnd_NDC;      lRayEnd_camera /= lRayEnd_camera.w;
		glm::vec4 lRayEnd_world = InverseViewMatrix * lRayEnd_camera;   lRayEnd_world /= lRayEnd_world.w;


		// Faster way (just one inverse)
		//glm::mat4 M = glm::inverse(ProjectionMatrix * ViewMatrix);
		//glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
		//glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;


		glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
		lRayDir_world = glm::normalize(lRayDir_world);


		out_origin = glm::vec3(lRayStart_world);
		out_direction = glm::normalize(lRayDir_world);
	}

	bool f1Pressed = false;
	bool debugDrawerOn = false;
    void processInput(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);




		if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
		{
			if (!f1Pressed)
			{
				debugDrawerOn = !debugDrawerOn;
				f1Pressed = true;
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_RELEASE)
		{
			f1Pressed = false;
		}
    }

    PhysicsObject* pickObject(Window* window, btDiscreteDynamicsWorld* dynamicsWorld, Camera* camera)
    {
		double mouseX, mouseY;
		glfwGetCursorPos(*window, &mouseX, &mouseY);


		glm::vec3 out_origin = { 0,0,0 };
		glm::vec3 out_direction = { 0,0,0 };
		glm::vec3 out_end = { 0,0,0 };

		if (glfwGetMouseButton(*window, GLFW_MOUSE_BUTTON_LEFT)) {

			std::vector<btVector3> hitpoints;
			std::vector<const btCollisionObject*> hitObjects;

			btVector3 hitPoint = btVector3(0, 0, 0);


			ScreenPosToWorldRay(
				mouseX, window->height - mouseY,
				window->width, window->height,
				camera->view,
				camera->projection,
				out_origin,
				out_direction
			);

			out_end = out_origin + out_direction * 1000.0f;

			btCollisionWorld::AllHitsRayResultCallback RayCallback(
				btVector3(out_origin.x, out_origin.y, out_origin.z),
				btVector3(out_end.x, out_end.y, out_end.z)
			);

			dynamicsWorld->rayTest(
				btVector3(out_origin.x, out_origin.y, out_origin.z),
				btVector3(out_end.x, out_end.y, out_end.z),
				RayCallback
			);

			if (RayCallback.hasHit()) {
				// Store the ray origin for distance calculation
				btVector3 rayOrigin = RayCallback.m_rayFromWorld;

				// Store hit points with distance to origin
				std::vector<std::pair<btVector3, float>> hitPointsWithDistance;
				std::vector<std::pair<const btCollisionObject*, float>> objectsWithDistance;

				for (int i = 0; i < RayCallback.m_hitPointWorld.size(); ++i) {
					btVector3 point = RayCallback.m_hitPointWorld[i];
					float distance = (point - rayOrigin).length();
					hitPointsWithDistance.push_back(std::make_pair(point, distance));
					objectsWithDistance.push_back(std::make_pair(RayCallback.m_collisionObjects[i], distance));
				}

				// Sort hit points by distance
				std::sort(hitPointsWithDistance.begin(), hitPointsWithDistance.end(),
					[](const std::pair<btVector3, float>& a, const std::pair<btVector3, float>& b)
					{
						return a.second < b.second;
					}
				);

				std::sort(objectsWithDistance.begin(), objectsWithDistance.end(),
					[](const std::pair<const btCollisionObject*, float>& a, const std::pair<const btCollisionObject*, float>& b)
					{
						return a.second < b.second;
					}
				);

				hitpoints.clear();
				for (const auto& hit : hitPointsWithDistance) {
					hitpoints.push_back(hit.first);
				}

				hitObjects.clear();
				for (const auto& hit : objectsWithDistance) {
					hitObjects.push_back(hit.first);
				}

				return (PhysicsObject*)hitObjects[0]->getUserPointer();
			}
		}

		return nullptr;
    }


	bool getDebugDrawerOn()
	{
		return debugDrawerOn;
	}
}
