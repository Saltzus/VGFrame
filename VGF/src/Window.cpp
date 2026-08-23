#include "Window.h"
#include "Renderer.h"

#include "graphicsApi/opengl/OpenGlManager.h"

namespace VGF
{

	Window::~Window()
	{
		glfwTerminate();
	}

	/// @param WinName = Title of the window
	/// @param WinWidth = Width of the window
	/// @param WinHeight = Height of the window
	/// @param Monitor = The monitor to use for full screen mode, or 'NULL' for windowed mode.
	/// @param window = Share The window whose context to share resources with, or 'NULL' to not share resources.
	/// \returns GLFWwindow*
	Window::Window(const char* WinName, int WinWidth, int WinHeight, GLFWmonitor* monitor, GLFWwindow* window)
	{
		width = WinWidth;
		height = WinHeight;

		if (!glfwInit()) {
			throw std::runtime_error("glfwInit() failed");
		}

		glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

		if (Renderer::GetGraphicsApi() == GraphicsApis::Vulkan)
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}
		if (Renderer::GetGraphicsApi() == GraphicsApis::OpenGL)
		{
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		}

		GLFW_Window = glfwCreateWindow(WinWidth, WinHeight, WinName, monitor, window);
		glfwSetWindowUserPointer(GLFW_Window, this);

		if (GLFW_Window == NULL)
		{
			std::cout << "Failed to create GLFW window  " << std::endl;
			glfwTerminate();
		}

		glfwMakeContextCurrent(GLFW_Window);
		glfwSetFramebufferSizeCallback(GLFW_Window, framebuffer_size_callback);

		if (Renderer::GetGraphicsApi() == GraphicsApis::OpenGL)
		{
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				std::cout << "Failed to initialize GLAD" << std::endl;
			}

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_FRAMEBUFFER_SRGB);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//glClipControl(GL_UPPER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
		}

		Renderer::InitGraphics(GLFW_Window);
	}

	void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (windowInstance)
		{
			windowInstance->onResize(width, height);
		}

		if (Renderer::GetGraphicsApi() == GraphicsApis::OpenGL)
		{
			glViewport(0, 0, width, height);
		}
	}

	void Window::window_size_callback(GLFWwindow* glfwWindow, int width, int height)
	{
		Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
		if (windowInstance)
		{
			windowInstance->onResize(width, height);
		}
	}

	void Window::Display()
	{
		glfwSwapBuffers(GLFW_Window);
		glfwPollEvents();

		if (Renderer::GetGraphicsApi() == GraphicsApis::Vulkan)
		{
		}
		if (Renderer::GetGraphicsApi() == GraphicsApis::OpenGL)
		{
#ifdef NDEBUG
			glClearColor(0, 0, 0, 0);
#else
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
#endif
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindFramebuffer(GL_FRAMEBUFFER, Opengl::Opengl::openglInstance->framebuffer);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}

	void Window::Update()
	{
		glfwSetWindowSizeCallback(this->GLFW_Window, window_size_callback);
		int win_width, win_height;
		glfwGetFramebufferSize(this->GLFW_Window, &win_width, &win_height);
		glViewport(0, 0, win_width, win_height);
	}

	void Window::onResize(int width, int height) {
		this->width = width;
		this->height = height;
		this->resized = true;

		for (auto callback : resizeCallbacks) callback();
	}
}