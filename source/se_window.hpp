#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULCAN
#include <GLFW/glfw3.h>
#include <string>

namespace se 
{
	class SeWindow 
	{
	public:
		SeWindow(int w, int h, std::string name);
		~SeWindow();

		SeWindow(const SeWindow&) = delete;
		SeWindow& operator=(const SeWindow&) = delete;
		SeWindow(SeWindow&&) = default;
		SeWindow& operator=(SeWindow&&) = default;

		bool shouldClose() { return glfwWindowShouldClose(window); }
		VkExtent2D getExtent() const { return { static_cast<uint32_t>(width),  static_cast<uint32_t>(height) }; };
		bool wasWindowResized() { return framebufferResized; };
		void resetWindowResizedFlag() { framebufferResized = false; };
		GLFWwindow* getGLFWwindow() const { return window; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		void InitWindow();

		int width;
		int height;
		bool framebufferResized = false;

		std::string windowName;
		GLFWwindow* window;
	};
}