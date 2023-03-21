#include "Camera.h"
#include <GLFW/glfw3.h>

// 默认相机属性
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

Camera::Camera(glm::vec3 InPos, glm::vec3 InWorldUp, float InYaw, float InPitch)
{
	this->Pos = InPos;
	this->WorldUp = InWorldUp;
	this->Yaw = InYaw;
	this->Pitch = InPitch;

	this->Front = glm::vec3(0.0f, 0.0f, -1.0f);
	this->MovementSpeed = SPEED;
	this->MouseSensitivity = SENSITIVITY;
	this->Zoom = ZOOM;

	this->UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
	glm::vec3 tempFront;
	tempFront.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	tempFront.y = sin(glm::radians(this->Pitch));
	tempFront.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	this->Front = glm::normalize(tempFront);

	this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
	this->Up = glm::normalize(glm::cross(this->Right, this->Front));
}

glm::mat4 Camera::LookAt()
{
	return glm::lookAt(this->Pos, this->Pos + this->Front, this->Up);
}

void Camera::ProcessKeyboard(EMoveDir direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;
	switch (direction)
	{
	case EMoveDir::FORWARD:
		Pos += Front * velocity;
		break;
	case EMoveDir::BACKWARD:
		Pos -= Front * velocity;
		break;
	case EMoveDir::LEFT:
		Pos -= Right * velocity;
		break;
	case EMoveDir::RIGHT:
		Pos += Right * velocity;
		break;
	default:
		break;
	}
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool bConstrainPitch)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	if (bConstrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	Zoom -= (float)yoffset;

	if (Zoom < 1.0f)
	{
		Zoom = 1.0f;
	}
	else if (Zoom > 45.0f)
	{
		Zoom = 45.0f;
	}
}
