#include "se_window.hpp"

#include <stdexcept>

namespace se 
{
	SeWindow::SeWindow(int w, int h, std::string name)
		: width{ w }, height{ h }, windowName{ name }
	{
		InitWindow();
	}

	SeWindow::~SeWindow()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void SeWindow::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void SeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create window surface");
		}
	}

	void SeWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto seWindow = reinterpret_cast<SeWindow*>(glfwGetWindowUserPointer(window));
		seWindow->framebufferResized = true;
		seWindow->width = width;
		seWindow->height = height;
	}
}	