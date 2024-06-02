#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include <iostream>

// imporvement : try do manage split screen

struct Camera
{
	float mouseSensitivity;
	glm::vec2 oldMousePostion;
	glm::vec3 eye;
	glm::vec3 viewDirection;
	glm::vec3 upVector;

	glm::mat4 GetViewMatrix(void);
	void MouseLook(int mouseX, int mouseY);
	void MoveForward(float speed);
	void MoveBackward(float speed);
	void MoveLeft(float speed);
	void MoveRight(float speed);

};


#endif
