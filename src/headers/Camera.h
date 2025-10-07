#ifndef CAMERA_H
#define CAMERA_H

#include <SDL.h>

class Camera {
public:
   Camera(float yaw);
   void Render();
   void HandleEvent(SDL_Event* event, double deltaTime);
   float GetYaw() const { return yaw; }
   float GetPitch() const { return pitch; }

private:
   float yaw;
   float pitch;
   float mouse_sensitivity = 0.1f;
};

#endif