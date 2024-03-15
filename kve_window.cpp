#include "kve_window.hpp"
#include <stdexcept>

namespace kve {

	KveWindow::KveWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } {
		initWindow();
	}

	KveWindow::~KveWindow()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void KveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface");
		}
	}

	void KveWindow::frameBufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto kveWindow = reinterpret_cast<KveWindow*>(glfwGetWindowUserPointer(window));
		kveWindow->frameBufferResized = true;
		kveWindow->width = width;
		kveWindow->height = height;
	}

	void KveWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
	}

}