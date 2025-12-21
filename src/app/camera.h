#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera
{
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 up;

    float yaw;
    float pitch;
};

void Camera_Init(Camera* camera, const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up)
{
    camera->position = position;
    camera->forward = forward;
    camera->up = up;
}

void Camera_Update_Vectors(Camera* camera)
{
    // Clamp pitch so we can't look inside our own body
    if (camera->pitch > 89.0f) camera->pitch = 89.0f;
    if (camera->pitch < -89.0f) camera->pitch = -89.0f;

    // 1. Calculate new Forward vector using Spherical Coordinates conversion
    glm::vec3 front;
    front.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
    front.y = sin(glm::radians(camera->pitch));
    front.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
    camera->forward = glm::normalize(front);
}

// Generate the View Matrix for the shader
glm::mat4 Camera_GetViewMatrix(const Camera* cam) {
    // lookAt( where_we_are, target_point, up_direction )
    return glm::lookAt(cam->position, cam->position + cam->forward, cam->up);
}