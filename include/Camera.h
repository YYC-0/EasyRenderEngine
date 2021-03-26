#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

enum class CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

enum class CameraType
{
	Perpective,
	Orthogonal
};

class Camera
{
public:
	// Perpective
	Camera(float aspect,
		glm::vec3 position,
		float fov = 45.0f,
		float yaw = -90.0f,
		float pitch = 0.0f, 
		float speed = 4.5f, 
		float sensitivity = 0.5f,
		float near = 0.1f,
		float far = 100.0f); // 弃用
	Camera(float aspect,
		glm::vec3 position,
		glm::vec3 viewDirection,
		float fov = 45.0f,
		float speed = 4.5f,
		float sensitivity = 0.5f, // (0, 1.0]
		float near = 0.2f,
		float far = 100.0f);
	// Orthogonal 
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
		float orthoWidth = 10.0f,
		float orthoHeight = 10.0f,
		float yaw = -90.0f,
		float pitch = 0.0f,
		float speed = 4.5f,
		float sensitivity = 0.1f,
		float near = 0.1f,
		float far = 100.0f);
	//Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
	//	float orthoWidth = 10.0f,
	//	float orthoHeight = 10.0f,
	//	glm::vec3 front = glm::vec3(0,0,0),
	//	float speed = 4.5f,
	//	float sensitivity = 0.1f,
	//	float near = 0.1f,
	//	float far = 100.0f);

	// 使用标量的构造函数
	//Camera(float posX, float posY, float posZ, 
	//	float upX, float upY, float upZ, float yaw, float pitch);

	void setMovingSpeed(float speed) {
		this->movementSpeed = speed;
	}

	// 返回lookAt矩阵
	glm::mat4 getViewMatrix() const {
		return glm::lookAt(position, position + front, up);
		//return glm::lookAt(position, front, worldUp);
	}
	glm::mat4 getProjectionMatrix() const {
		return projectionMatrix;
	}

	glm::vec3 getPos() const { return position; }
	glm::vec3 getViewDir() const { return front; }

	// 处理键盘输入
	void processKeyboard(CameraMovement direction, float deltaTime);

	// 处理鼠标移动
	void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

	// 处理鼠标滚轮
	void processMouseScroll(float yoffset);


private:
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	// matrix
	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;

	// 摄影机属性
	CameraType projectionType;
	glm::vec3 position;
	glm::vec3 worldUp;

	// 欧拉角
	float yaw; // 偏航角
	float pitch; // 俯仰角

	// 摄像机选项
	float movementSpeed;
	float mouseSensitivity;
	float fov;

	// 视锥
	float near;
	float far;
	// 透视投影
	float aspect;
	// 正交投影
	float orthoWidth;
	float orthoHeight;

	// 从欧拉角计算 front 向量
	void updateCameraVectors();
};
