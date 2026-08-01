#include "Camera.h"
#include "Window.h"

namespace VGF
{
	Camera::Camera(glm::vec3 position)
	{
		this->position = -position;
	}

	void Camera::updateMatrix(const VGF::Window& window, float FOVdeg, float nearPlane, float farPlane)
	{
		// Initializes matrices since otherwise they will be the null matrix
		view = glm::mat4(1.0f);
		projection = glm::mat4(1.0f);

		// Makes camera look in the right direction from the right position
		view = glm::lookAt(position, position + orientation, up);
		// Adds perspective to the scene

        if (window.width != 0 || window.height != 0)
		    projection = glm::perspective(glm::radians(FOVdeg), (float)window.width / window.height, nearPlane, farPlane);

		// Sets new camera matrix
		cameraMatrix = projection * view;
	}

    double lastMouseX, lastMouseY;

    void Camera::Inputs(const VGF::Window& window, double deltatime)
    {
        float frameSpeed = speed * 100 * static_cast<float>(deltatime);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) position += frameSpeed * orientation;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) position += frameSpeed * -glm::normalize(glm::cross(orientation, up));
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) position += frameSpeed * -orientation;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) position += frameSpeed * glm::normalize(glm::cross(orientation, up));
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) position += frameSpeed * up;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) position += frameSpeed * -up;

        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            speed = 0.2f;
        }
        else {
            speed = 0.07f;
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            if (firstClick)
            {
                lastMouseX = mouseX;
                lastMouseY = mouseY;
                firstClick = false;
            }

            float rotX = sensitivity * (float)(mouseY - lastMouseY) / window.height;
            float rotY = sensitivity * (float)(mouseX - lastMouseX) / window.width;

            lastMouseX = mouseX;
            lastMouseY = mouseY;

            glm::vec3 newOrientation = glm::rotate(orientation,
                glm::radians(-rotX),
                glm::normalize(glm::cross(orientation, up)));

            if (abs(glm::angle(newOrientation, up) - glm::radians(90.0f)) <= glm::radians(85.0f))
                orientation = newOrientation;

            orientation = glm::rotate(orientation, glm::radians(-rotY), up);
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            firstClick = true;
        }
    }
}