#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

//Our headers
#include "Camera.hpp"

/* struct Object
{
	std::vector<GLfloat> vertexData;
	GLuint numVertex;
	std::vector<GLuint> indexData;
	GLuint numIndex;
}; */

struct Window
{
	int screenWidth;
	int screenHeight;
	SDL_Window * window;
	SDL_GLContext context;
	bool running;
	// VAO
	GLuint vertexArrayObject;
	// VBO
	GLuint vertexBufferObject;
	GLuint indexBufferObject;
	// Program Object (for our shaders)
	GLuint graphicsPipelineShaderProgram;

	float uOffset;
	float uRotation;
	float uScale;
};

// Error Handling
static void GLClearAllErrors(void)
{
	while (glGetError() != GL_NO_ERROR)
	{

	}
}

static bool GLCheckErrorStatus(const char *function, int line)
{
	while (GLenum error = glGetError())
	{
		std::cerr << "OpenGL Error: " << error << "\tLine: " << line << "\tfunction: " << function << std::endl;
		return (true);
	}
	return (false);
}

#define GLCheck(x) GLClearAllErrors(); x; GLCheckErrorStatus(#x, __LINE__);

GLuint CompileShader(GLuint type, const std::string & source)
{
	GLuint shaderObject;

	if (type == GL_VERTEX_SHADER)
		shaderObject = glCreateShader(GL_VERTEX_SHADER);
	else if (type == GL_FRAGMENT_SHADER)
		shaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const char * src = source.c_str();
	glShaderSource(shaderObject, 1, &src, nullptr);
	// Compile shader
	glCompileShader(shaderObject);

	int result;

	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		char *errorMessages = new char[length];
		glGetShaderInfoLog(shaderObject, length, &length, errorMessages);

		if (type == GL_VERTEX_SHADER)
			std::cout << "ERROR: GL_VERTEX_SHADER compilation failed!\n" << errorMessages << std::endl;
		else if (type == GL_FRAGMENT_SHADER)
			std::cout << "ERROR: GL_FRAGMENT_SHADER compilation failed!\n" << errorMessages << std::endl;
		delete[] errorMessages;

		glDeleteShader(shaderObject);

		return (0);
	}

	return (shaderObject);
}

GLuint CreateShaderProgram(const std::string & vertexShaderSource, const std::string & fragmentShaderSource)
{
	// Create a new program object
	GLuint programObject = glCreateProgram();

	// Compile our shaders
	GLuint myVertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
	GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	// Link our shaders to the shader program
	glAttachShader(programObject, myVertexShader);
	glAttachShader(programObject, myFragmentShader);
	glLinkProgram(programObject);

	// Validate our program
	glValidateProgram(programObject);

	// Detach and Delete our shaders
	glDetachShader(programObject, myVertexShader);
	glDetachShader(programObject, myFragmentShader);

	glDeleteShader(myVertexShader);
	glDeleteShader(myFragmentShader);

	return (programObject);
}

std::string LoadShaderAsString(const std::string &filename)
{
	// Resulting shader program loaded as a single string;
	std::string result = "";

	std::string line = "";
	std::ifstream shaderFile(filename.c_str());

	if (shaderFile.is_open())
	{
		while (std::getline(shaderFile, line))
		{
			result += line + '\n';
		}
		shaderFile.close();
	}
	else
		std::cerr << "ERROR: Loading shader file '" << filename << "'" << std::endl;
	return (result);
}

void CreateGraphicsPipeline(Window *window)
{

	std::string vertexShaderSource = LoadShaderAsString("./shaders/vert.glsl");
	std::string fragmentShaderSource = LoadShaderAsString("./shaders/frag.glsl");

	window->graphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);

}

void VertexSpecification(Window *window)
{
	// Geometry Data that lives on the CPU
	// vertex [-1.0f -> 1.0f]
	// color [0.0f -> 1.0f]
	// TODO: try to load object (vertex list) from file
	const std::vector<GLfloat> vertexData
	{
		// 1. Front face
		-0.5f, -0.5f, 0.5f,	// left vertex position
		1.0f, 0.0f, 0.0f,	// color
		0.5f, -0.5f, 0.5f,	// right vertex position
		1.0f, 0.0f, 0.0f,	// color
		//
		-0.5f, 0.5f, 0.5f,	// top left vertex position
		0.0f, 0.0f, 1.0f,	// color
		0.5f, 0.5f, 0.5f,	// top right vertex position
		0.0f, 0.0f, 1.0f,	// color
		// 2. Bottom face (only non dup vertices)
		-0.5f, -0.5f, -0.5f,	// back left vertex position
		1.0f, 0.0f, 0.0f,	// color
		0.5f, -0.5f, -0.5f,	// back right vertex position
		1.0f, 0.0f, 0.0f,	// color
		// 2. Top face
		-0.5f, 0.5f, -0.5f,	// back left vertex position
		0.0f, 0.0f, 1.0f,	// color
		0.5f, 0.5f, -0.5f,	// back right vertex position
		0.0f, 0.0f, 1.0f,	// color
	};

	// We start setting things up
	// on the GPU
	glGenVertexArrays(1, &window->vertexArrayObject);
	glBindVertexArray(window->vertexArrayObject);

	// Start generating out VBO
	glGenBuffers(1, &window->vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, window->vertexBufferObject);

	// Position information
	// Note:	vertexData.size() * sizeof(GLfloat) = sizeof(vertexData)
	//			vertexData.data() = vertexData
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat), vertexData.data(), GL_STATIC_DRAW);

	// CCW
	const std::vector<GLuint> indexBufferData{2, 0, 1, 3, 2, 1, 0, 1, 4, 4, 1, 5, 2, 3, 6, 6, 3, 7};
	// Setup the Index Buffer Object (IBO/EBO)
	glGenBuffers(1, &window->indexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, window->indexBufferObject);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferData.size() * sizeof(GLuint), indexBufferData.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(GLfloat) * 6 , (GLvoid*)0);

	// Now linking up the attributes in our VAO
	// Color information
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(GLfloat) * 6, (GLvoid*)(sizeof(GLfloat) * 3));

	// Unbinding our currently bound Vertex Array Object
	glBindVertexArray(0);
	// Disable any attributes we opened in our Vertex Atrribute Array
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

}

void InitializeProgram(Window *window)
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "Error with SDL_Init()" << std::endl;
		exit(1);
	}

	// Setup the OpenGL Context
	// Use OpenGL 4.1 core or greater
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// We want to request a double buffer for smooth updating
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);


	// Create an application window using OpenGL that supports SDL
	window->window = SDL_CreateWindow("OpenGL", 100, 100, window->screenWidth, window->screenHeight, SDL_WINDOW_OPENGL);
	// Check if Window did not create.
	if (window->window == nullptr)
	{
		std::cerr << "Error with SDL_CreateWindow()" << std::endl;
		exit(1);
	}


	// Create an OpenGL Graphics Context
	window->context = SDL_GL_CreateContext(window->window);
	if (window->context == nullptr)
	{
		std::cerr << "Error whit SDL_GL_CreateContext()" << std::endl;
		exit(1);
	}

	// Initialize Glad Library
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		std::cerr << "Error with gladLoadGLLoader()" << std::endl;
		exit(1);
	}

	return ;
}

void Input(Window *window, Camera *camera)
{
	static int mouseX = window->screenWidth / 2;
	static int mouseY = window->screenHeight / 2;
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
		if (event.type == SDL_QUIT)
		{
			std::cout << "Exiting!" << std::endl;
			window->running = false;
		}
		else if (event.type == SDL_MOUSEMOTION)
		{
			mouseX += event.motion.xrel;
			mouseY += event.motion.yrel;
			camera->MouseLook(mouseX, mouseY);
		}
	}

	// Retrieve keyboard state
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	float speed = 0.2f;

	if (state[SDL_SCANCODE_ESCAPE])
		window->running = false;
	if (state[SDL_SCANCODE_W])
	{
		camera->MoveForward(speed);
	}
	if (state[SDL_SCANCODE_S])
	{
		camera->MoveBackward(speed);
	}
	if (state[SDL_SCANCODE_D])
	{
		camera->MoveRight(speed);
	}
	if (state[SDL_SCANCODE_A])
	{
		camera->MoveLeft(speed);
	}
	if (state[SDL_SCANCODE_SPACE])
	{
		camera->eye.y += 0.5f;
	}
	return ;
}

void PreDraw(Window *window, Camera *camera)
{
	// Disable depth test and face culling
	// glDisable(GL_DEPTH_TEST);
	// Enabled it so we don't see something behind something else
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// Initialize clear color
	// This is the background of the screen.
	glViewport(0, 0, window->screenWidth, window->screenHeight);
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

	// Clear color buffer and depth buffer
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	// User our shader
	glUseProgram(window->graphicsPipelineShaderProgram);

	// Model transformation by translating our object into world space
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, window->uOffset));
	// Retrive the location of our Model Matrix
	GLint u_ModelMatrixLocation = glGetUniformLocation(window->graphicsPipelineShaderProgram, "u_ModelMatrix");
	if (u_ModelMatrixLocation < 0)
	{
		std::cout << "Location not found for 'u_ModelMatrix'" << std::endl;
		window->running = false;
	}

	glm::mat4 view = camera->GetViewMatrix();
	GLint u_ViewLocation = glGetUniformLocation(window->graphicsPipelineShaderProgram, "u_ViewMatrix");
	if (u_ViewLocation < 0)
	{
		std::cout << "Location not found for 'u_ViewMatrix'" << std::endl;
		window->running = false;
	}

	// Projection/Perspective matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)window->screenWidth/(float)window->screenHeight, 0.1f, 100.0f);
	GLint u_ProjectionLocation = glGetUniformLocation(window->graphicsPipelineShaderProgram, "u_Projection");
	if (u_ProjectionLocation < 0)
	{
		std::cout << "Location not found for 'u_Projection'" << std::endl;
		window->running = false;
	}

	// Rotation matrix
	model = glm::rotate(model,glm::radians(window->uRotation), glm::vec3(0.0f, 1.0f, 0.0f));
	// Scale matrix
	model = glm::scale(model, glm::vec3(window->uScale, window->uScale, window->uScale));

	glUniformMatrix4fv(u_ModelMatrixLocation, 1, false, &model[0][0]);
	glUniformMatrix4fv(u_ProjectionLocation, 1, false, &projection[0][0]);
	glUniformMatrix4fv(u_ViewLocation, 1, false, &view[0][0]);
}

void Draw(Window *window)
{
	// Enable our attributes
	glBindVertexArray(window->vertexArrayObject);

	// Render data (draw call)
	// glDrawArrays(GL_TRIANGLES, 0, 6);
	// the number of element should be replace by a variable
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

	// Stop using our current graphics pipeline
	// Note: This is not necessary if we only have one grpahics pipeline
	glUseProgram(0);
}

void MainLoop(Window *window, Camera *camera)
{
	// Set the mouse to the middle of the screen
	SDL_WarpMouseInWindow(window->window, window->screenWidth / 2, window->screenHeight / 2);

	SDL_SetRelativeMouseMode(SDL_TRUE);
	while (window->running)
	{
		window->uRotation += 0.3f;
		if (camera->eye.y > 0.0f)
			camera->eye.y -= 0.1f;
		if (camera->eye.y < 0.0f)
			camera->eye.y = 0.0f;
		if (window->uRotation >= 360.0f)
			window->uRotation -= 360.0f;

		Input(window, camera);

		PreDraw(window, camera);

		Draw(window);

		//Update the screen of our window
		SDL_GL_SwapWindow(window->window);
	}
}

void CleanUp(Window *window)
{
	// Probably missing something to have no leaks
	SDL_GL_DeleteContext(window->context);
	SDL_DestroyWindow(window->window);

	SDL_Quit();
}


int main(void)
{
	Window window = {1000, 800, NULL, NULL, true, 0, 0, 0, 0, -3.0f, 0.0f, 1.0f};
	// We are placed at the origin, looking in front (out into the scene), and the up is the y axis
	Camera camera = {0.2f, glm::vec2(window.screenWidth/2, window.screenHeight/2), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)};

	// 1. Setup the graphics program
	InitializeProgram(&window);

	// 2. Setup our geometry
	VertexSpecification(&window);

	// 3. Create our graphics pipeline
	// - At a minimum, this means the vertex and fragment shader
	CreateGraphicsPipeline(&window);

	//Main loop:
	//	Input
	//	Update
	//	Render
	// 4. Call the main application loop
	MainLoop(&window, &camera);

	// 5. Call the cleanup function when our program terminates
	CleanUp(&window);

	return (0);
}
