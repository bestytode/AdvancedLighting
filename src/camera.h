#pragma once

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum CameraMovementDirection
{
    FORWARD,  // W
    BACKWARD, // A
    LEFT,     // S
    RIGHT     // D
};

// An abstract camera class that processes input and calculates 
// the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    Camera(float _x, float _y, float _z)
        : position(glm::vec3(_x, _y, _z)),
        direction(glm::vec3(0.0f, 0.0f, -1.0f)),
        up(glm::vec3(0.0f, 1.0f, 0.0f)),
        worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
        yaw(-90.f), pitch(0.0f),
        movementSpeed(2.5f), mouseSensitivity(0.1f), fov(45.0f)
    {
        right = glm::normalize(glm::cross(direction, worldUp));
        UpdateCameraVectors();
    }

    Camera(glm::vec3 _position)
        : position(_position),
        direction(glm::vec3(0.0f, 0.0f, -1.0f)),
        up(glm::vec3(0.0f, 1.0f, 0.0f)),
        worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
        yaw(-90.f), pitch(0.0f),
        movementSpeed(2.5f), mouseSensitivity(0.1f), fov(45.0f)
    {
        right = glm::normalize(glm::cross(direction, worldUp));
        UpdateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(position, position + direction, up);
    }

    // processes input received from any keyboard-like input system. 
    // Accepts input parameter in the form of camera defined ENUM 
    void ProcessKeyboard(CameraMovementDirection _direction, float deltaTime);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrain = true);

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void UpdateCameraVectors();

public:
    // Camera Attributes
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    // Euler Angles
    float yaw;
    float pitch;

    // Camera options
    float movementSpeed;
    float mouseSensitivity;
    float fov; // field of view
};

void Camera::ProcessKeyboard(CameraMovementDirection _direction, float deltaTime)
{
	float velocity = movementSpeed * deltaTime;

	if (_direction == FORWARD)
		position += this->direction * velocity;
	if (_direction == BACKWARD)
		position -= this->direction * velocity;
	if (_direction == LEFT)
		position -= this->right * velocity;
	if (_direction == RIGHT)
		position += this->right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrain)
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;
    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds
    if (constrain) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    // update Front, Right and Up Vectors
    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

void Camera::UpdateCameraVectors()
{
    // calculate the new direction vector
    glm::vec3 _direction;
    _direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    _direction.y = sin(glm::radians(pitch));
    _direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->direction = glm::normalize(_direction);

    // Also re-calculate the Right and Up vector
    this->right = glm::normalize(glm::cross(direction, worldUp));
    this->up = glm::normalize(glm::cross(right, direction));
}