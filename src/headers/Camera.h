#ifndef CAMERA_H
#define CAMERA_H

#include <SDL.h>

#include "vec2.h"
#include "vec3.h"

class Camera {
public:
   Camera(float yaw);
   void Render();
   void HandleEvent(SDL_Event* event, double deltaTime);
   float GetYaw() const { return yaw; }
   float GetPitch() const { return pitch; }

   fvec2 get2DVector();
   fvec3 get3DVector();


private:
   float yaw;
   float pitch;
   float mouse_sensitivity = 0.1f;
};

#endif