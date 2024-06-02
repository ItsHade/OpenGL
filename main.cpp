//GLFW
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

GLFWwindow *Init(void)
{
	GLFWwindow *window;
	// Initialize glfw library
	if (!glfwInit())
	{
		std::cerr << "Error with glfwInit()" << std::endl;
		exit(1);
	}

	// Create a (windowed mode) window
	window = glfwCreateWindow(1000, 800, "OpenGL Game", NULL, NULL);
	if (!window)
	{
		std::cerr << "Error with glfwCreateWindow()" << std::endl;
		glfwTerminate();
		exit (1);
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	return (window);

}

int main(void)
{
	GLFWwindow *window = Init();

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwTerminate();
	return (0);
}
