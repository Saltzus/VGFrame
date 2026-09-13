#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Shader.h"


namespace VGF
{
	class Window;
    class Camera
    {
    public:
		glm::vec3 position;
		glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::mat4 cameraMatrix = glm::mat4(1.0f);

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		// Prevents the camera from jumping around when first clicking left click
		bool firstClick = true;

		// Adjust the speed of the camera and it's sensitivity when looking around
		float speed = 1.f;
		float sensitivity = 100.0f;

		// Camera constructor to set up initial values
		Camera(glm::vec3 position = glm::vec3(0.f));

		// Updates the camera matrix to the Vertex Shader
		void updateMatrix(const Window& window, float FOVdeg, float nearPlane, float farPlane);
		// Exports the camera matrix to a shader
		void Matrix(Shader& shader, const char* uniform);
		// Handles camera inputs
		void Inputs(const Window& window, double deltatime);

    private:
    	bool _perspective;
    	bool _perspectiveCheck = false;
    };

}