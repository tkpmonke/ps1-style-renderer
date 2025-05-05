/// included for GLEW
#include "camera.h"
#include "window.h"

#include "GLFW/glfw3.h"

void create_window(window_t* window, const char* name) {
	glfwInit();
	window->window = glfwCreateWindow(window->width, window->height, name, NULL, NULL);
	glfwMakeContextCurrent(window->window);
	
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

char poll_window_events(window_t* window) {
	glfwPollEvents();
	glfwGetWindowSize(window->window, &window->width, &window->height);
	return !glfwWindowShouldClose(window->window);
}

void present_window(window_t* window) {
	glfwSwapBuffers(window->window);
}

void destroy_window(window_t* window) {
	glfwDestroyWindow(window->window);
	glfwTerminate();
}
