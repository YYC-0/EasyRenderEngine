#include "../include/Camera.h"
#include <iostream>
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

Camera::Camera(float aspect_, glm::vec3 position_, glm::vec3 viewDirection, float fov_, float speed, float sensitivity, float near_, float far_) :
	aspect(aspect_),
	position(position_),
	fov(fov_),
	movementSpeed(speed),
	near(near_),
	far(far_)
{
	projectionType = CameraType::Perpective;
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	front = glm::normalize(viewDirection);

	updateCameraVectors();

	projectionMatrix = glm::perspective(glm::radians(fov), aspect, near, far);

	if (sensitivity > 1.0)
		sensitivity = 1.0;
	if (sensitivity < 0)
		sensitivity = 0.01;
	mouseSensitivity = sensitivity * 0.002;
}

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

}

void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	glm::mat4 trans = glm::mat4(1.0f);
	if ((front.y < 0.99 && yoffset > 0) || (front.y > -0.99 && yoffset < 0))
		trans = glm::rotate(trans, yoffset, right);
	trans = glm::rotate(trans, -xoffset, glm::vec3(0.0, 1.0, 0.0));
	glm::vec4 tempFront(front, 1.0);
	tempFront = trans * tempFront;
	front = tempFront;

	//yaw += xoffset;
	//pitch += yoffset;

	//if (constrainPitch)
	//{
	//	if (pitch > 89.0f)
	//		pitch = 89.0f;
	//	if (pitch < -89.0f)
	//		pitch = -89.0f;
	//}

	updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset)
{
	float tmpSpeed = movementSpeed + yoffset;
	if(tmpSpeed > 0)
		movementSpeed = tmpSpeed;
}

void Camera::updateCameraVectors()
{
	//glm::vec3 tmpFront;
	//tmpFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	//tmpFront.y = sin(glm::radians(pitch));
	//tmpFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	//front = glm::normalize(tmpFront); // µ¥Î»»¯
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));

	//viewMatrix = glm::lookAt(position, position + front, up);
}
