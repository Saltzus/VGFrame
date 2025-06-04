#include "Camera.h"

namespace Realgar
{
    Camera::Camera(glm::vec3 position, glm::vec3 rotation, float fov, float nearPlane, float farPlane, bool ortho)
    {
    	cameraPosition = -position;
        cameraRotation = rotation;

        this->fov = fov;
        this->nearPlane = nearPlane;
        this->farPlane = farPlane;
        this->ortho = ortho;
    }

    void Camera::updateMatrix(GLFWwindow* window)
    {
    	view = glm::mat4(1.0f);
    	projection = glm::mat4(1.0f);

        if (Window::editor)
        {
            width = Window::viewportWidth;
            height = Window::viewportHeight;
        }
        else
            glfwGetFramebufferSize(window, &width, &height);

        if (Renderer::GetGraphicsApi() == GraphicsApis::OpenGL && ortho)
        {
            float scale = 0.024f; // or any desired scale factor
            float target_width = 640.0f * scale;
            float target_height = 360.0f * scale;
            float target_aspect = target_width / target_height;
            float aspect = (float)width / height;

            if (aspect > target_aspect) {
                float view_width = target_height * aspect;
                projection = glm::ortho(
                    -view_width / 2.0f, view_width / 2.0f,
                    -target_height / 2.0f, target_height / 2.0f,
                    nearPlane, farPlane
                );
            }
            else {
                float view_height = target_width / aspect;
                projection = glm::ortho(
                    -target_width / 2.0f, target_width / 2.0f,
                    -view_height / 2.0f, view_height / 2.0f,
                    nearPlane, farPlane
                );
            }
        }

        if (!ortho) projection = glm::perspective(glm::radians(fov), (float)width / height, nearPlane, farPlane);



        glm::vec3 front;
        front.x = cos(glm::radians(cameraRotation.x)) * cos(glm::radians(cameraRotation.y + 90));
        front.y = sin(glm::radians(cameraRotation.x));
        front.z = cos(glm::radians(cameraRotation.x)) * sin(glm::radians(cameraRotation.y + 90));
        cameraFront = glm::normalize(front);

        glm::vec3 right = glm::normalize(glm::cross(cameraFront, worldUp));
        glm::vec3 up = glm::normalize(glm::cross(right, cameraFront));
        cameraUp = glm::rotate(up, glm::radians(cameraRotation.z), cameraFront);

        view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
    }

}