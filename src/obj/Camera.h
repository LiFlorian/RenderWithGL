#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum EMoveDir {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera
{
public:
	// camera options
	float MovementSpeed;

	float MouseSensitivity;

	float Zoom;

	glm::vec3 Pos;

	glm::vec3 Front;

	glm::vec3 Up;

	glm::vec3 Right;

	glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// euler Angles
	float Yaw;

	float Pitch;

public:
	Camera(glm::vec3 InPos, glm::vec3 InWorldUp, float InYaw = -90.0f, float InPitch = 0.0f);

	void UpdateCameraVectors();

	glm::mat4 LookAt();

	// 键盘输入方向
	void ProcessKeyboard(EMoveDir direction, float deltaTime);

	// 鼠标滑动调整方向
	void ProcessMouseMovement(float xoffset, float yoffset, bool bConstrainPitch = true);

	// 鼠标滑动更改视野
	void ProcessMouseScroll(float yoffset);
};

