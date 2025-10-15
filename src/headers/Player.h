#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <SDL_opengl.h>
#include "Camera.h"
#include "new_models.h"

class Player {
public:
   Player(NewModels::vec3 position, float angle);
   void HandleEvent(SDL_Event* event, double deltaTime);
   void Render();

private:
   NewModels::vec3 position{}; // Short for map compatibility
   float pos_x, pos_y, pos_z; // Float for smooth movement
   Camera* camera;
   original_map * map;
   NewModels::Sector * current_sector;
};

#endif