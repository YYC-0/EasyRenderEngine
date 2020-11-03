#include "../include/Camera.h"

// Perpective
Camera::Camera(float aspect_, glm::vec3 position_, float fov_,
	float yaw_, float pitch_,
	float speed, float sensitivity,
	float near_, float far_) :
	aspect(aspect_),
	position(position_),
	fov(fov_),
	yaw(yaw_),
	pitch(pitch_),
	movementSpeed(speed),
	mouseSensitivity(sensitivity),
	near(near_),
	far(far_)
{
	projectionType = CameraType::Perpective;
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	updateCameraVectors();

	projectionMatrix = glm::perspective(glm::radians(fov), aspect, near, far);
}

Camera::Camera(glm::vec3 position_, 
	float orthWidth_, float orthHeight_, 
	float yaw_, float pitch_,
	float speed, float sensitivity,
	float near_, float far_) :
	position(position_),
	orthoWidth(orthWidth_),
	orthoHeight(orthHeight_),
	yaw(yaw_),
	pitch(pitch_),
	movementSpeed(speed),
	mouseSensitivity(sensitivity),
	near(near_),
	far(far_)
{
	projectionType = CameraType::Orthogonal;
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	updateCameraVectors(); 
	
	projectionMatrix = glm::ortho(-orthoWidth/2.0f, orthoWidth/2.0f, -orthoHeight/2.0f, orthoHeight/2.0f, near, far);
}
//
//Camera::Camera(glm::vec3 position_,
//	float orthWidth_, float orthHeight_,
//	glm::vec3 front_ = glm::vec3(0, 0, 0),
//	float speed, float sensitivity,
//	float near_, float far_) :
//	position(position_),
//	orthoWidth(orthWidth_),
//	orthoHeight(orthHeight_),
//	front(front_),
//	movementSpeed(speed),
//	mouseSensitivity(sensitivity),
//	near(near_),
//	far(far_)
//{
//	projectionType = CameraType::Orthogonal;
//	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
//	updateCameraVectors();
//
//	projectionMatrix = glm::ortho(-orthoWidth / 2.0f, orthoWidth / 2.0f, -orthoHeight / 2.0f, orthoHeight / 2.0f, near, far);
//}

void Camera::processKeyboard(CameraMovement direction, float deltaTime)
{
	float velocity = movementSpeed * deltaTime;
	if (direction == CameraMovement::FORWARD)
		position += front * velocity;
	else if (direction == CameraMovement::BACKWARD)
		position -= front * velocity;
	else if (direction == CameraMovement::LEFT)
		position -= right * velocity;
	else if (direction == CameraMovement::RIGHT)
		position += right * velocity;

	// 使摄像机固定在xz平面上
	// position.y = 0;
}

void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (constrainPitch)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}

	updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

void Camera::updateCameraVectors()
{
	glm::vec3 tmpFront;
	tmpFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	tmpFront.y = sin(glm::radians(pitch));
	tmpFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(tmpFront); // 单位化
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));

	//viewMatrix = glm::lookAt(position, position + front, up);
}
