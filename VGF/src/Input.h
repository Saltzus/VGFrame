#pragma once

#include "Window.h"
#include "Camera.h"

#include "PhysicsObject.h"

namespace VGF::Input
{
    void processInput(GLFWwindow* window);
	bool pressedKey(GLFWwindow* window, int key);
	bool pressedMouseButton(GLFWwindow* window, int button);

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


	template<typename T>
	T* pickObject(Window* window, btDiscreteDynamicsWorld* dynamicsWorld, Camera* camera)
	{
		double mouseX, mouseY;
		glfwGetCursorPos(*window, &mouseX, &mouseY);

		glm::vec3 out_origin = { 0,0,0 };
		glm::vec3 out_direction = { 0,0,0 };
		glm::vec3 out_end = { 0,0,0 };

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

			return (T*)hitObjects[0]->getUserPointer();
		}

		return nullptr;
	}

}

