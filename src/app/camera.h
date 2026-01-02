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

    float width;
    float height;
    float aspect_ratio;

    float fov;
    float near_plane;
    float far_plane;

    float movement_speed;
    float mouse_sensitivity;
};

namespace camera
{

void Init(Camera* camera, const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up, float width, float height)
{
    camera->position = position;
    camera->forward = forward;
    camera->up = up;

    camera->yaw = -90.0f; // Facing towards negative Z
    camera->pitch = 0.0f;

    camera->width = width;
    camera->height = height;
    camera->aspect_ratio = width / height;
    camera->fov = 45.0f;
    camera->near_plane = 0.1f;
    camera->far_plane = 100.0f;
}

void UpdateDimensions(Camera* camera, float width, float height)
{
    camera->width = width;
    camera->height = height;
    camera->aspect_ratio = width / height;
}

void UpdateVectors(Camera* camera)
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
glm::mat4 GetViewMatrix(const Camera* cam)
{
    // lookAt( where_we_are, target_point, up_direction )
    return glm::lookAt(cam->position, cam->position + cam->forward, cam->up);
}

glm::mat4 GetProjectionMatrix(const Camera* cam)
{
    return glm::perspective(glm::radians(cam->fov), cam->aspect_ratio, cam->near_plane, cam->far_plane);
}

} // namespace camera