#include "headers/Camera.h"
#include <SDL_opengl.h>
#include <cmath>
#define M_PI 3.14159265358979323846

Camera::Camera(float yaw) : yaw(yaw), pitch(0.0f) {}

void Camera::Render() {
    glRotatef(pitch, 1.0f, 0.0f, 0.0f); // Pitch (X-axis)
    glRotatef(yaw, 0.0f, 1.0f, 0.0f);  // Yaw (Y-axis)
}

void Camera::HandleEvent(SDL_Event* event, double deltaTime) {
    yaw -= event->motion.xrel * mouse_sensitivity * deltaTime * 60.0f;
    pitch -= event->motion.yrel * mouse_sensitivity * deltaTime * 60.0f;
    yaw = fmod(yaw+360.0f, 360.0f);
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}

fvec2 Camera::get2DVector() const {
    float rad_yaw = yaw * M_PI / 180.0f; // Convert degrees to radians
    return fvec2{
        sinf(rad_yaw),
        cosf(rad_yaw)
    };
}

fvec3 Camera::get3DVector() const {
    float rad_yaw = yaw * M_PI / 180.0f; // Convert degrees to radians
    float rad_pitch = pitch * M_PI / 180.0f;

    return fvec3{
        cosf(rad_pitch) * sinf(rad_yaw), // x
        sinf(rad_pitch), // y (up vector)
        cosf(rad_pitch) * cosf(rad_yaw) // z
    };
}
