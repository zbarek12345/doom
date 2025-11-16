#include "headers/Camera.h"
#include <SDL_opengl.h>

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