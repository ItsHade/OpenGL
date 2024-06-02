#include "Camera.hpp"

glm::mat4 Camera::GetViewMatrix(void)
{
	return (glm::lookAt(eye, eye + viewDirection, upVector));
}

void Camera::MouseLook(int mouseX, int mouseY)
{
	// need to rework to remove all static variables (Input())
	// not clean
	// std::cout << "mouse: " << mouseX << "," << mouseY << std::endl;

	static bool firstLook = true;
	glm::vec2 currentMouse = glm::vec2(mouseX, mouseY);

	if (firstLook)
	{
		oldMousePostion = currentMouse;
		firstLook = false;
	}

	glm::vec2 mouseDelta = oldMousePostion - currentMouse;
	viewDirection = glm::rotate(viewDirection, glm::radians((float)mouseDelta.x * mouseSensitivity), upVector);
	// doesn't quite work
	// viewDirection = glm::rotate(viewDirection, glm::radians((float)mouseDelta.y * mouseSensitivity), glm::vec3(1.0f, 0.0f, 0.0f));
	oldMousePostion = currentMouse;
}

void Camera::MoveForward(float speed)
{
	eye += (viewDirection * speed);
}

void Camera::MoveBackward(float speed)
{
	eye -= (viewDirection * speed);
}

void Camera::MoveLeft(float speed)
{
	eye -= glm::cross(viewDirection, upVector) * speed;
}

void Camera::MoveRight(float speed)
{
	eye += glm::cross(viewDirection, upVector) * speed;
}
