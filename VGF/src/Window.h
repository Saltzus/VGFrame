#pragma once

#include <functional>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace VGF 
{
	class Window
	{
	public:
		unsigned int ID;
		GLFWwindow* GLFW_Window;

		Window
		(
			const char* WinName,
			int WinWidth,
			int WinHeight,
			GLFWmonitor* monitor = NULL, 
			GLFWwindow* window = NULL
		);

		~Window();

		void Display();
		void Update();

		void AddWindowResizeCallback(std::function<void()> windowResizeCallback) {
			resizeCallbacks.push_back(windowResizeCallback);
		}

		int width = 0;
		int height = 0;
		bool resized = false;

		operator GLFWwindow* () const 
		{
			return GLFW_Window;
		}

	private:
		std::vector<std::function<void()>> resizeCallbacks;

		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		static void window_size_callback(GLFWwindow* glfwWindow, int width, int height);
		void onResize(int width, int height);
	};
}