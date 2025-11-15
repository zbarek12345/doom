#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <SDL_opengl.h>
#include "Camera.h"
#include "map.h"
#include "new_models.h"

class Player {
public:
   Player(NewModels::vec3 position, float angle, NewModels::Map* map);
   void Update(double deltaTime);
   void HandleEvent(SDL_Event* event, double deltaTime);
   void Render();

private:
   NewModels::fvec3 movement_vector{};
   NewModels::Map* new_map;
   NewModels::vec3 position{}; // Short for map compatibility
   NewModels::fvec3 pos;// Float for smooth movement
   Camera* camera;
   original_map * map;
   NewModels::Sector * current_sector;
};

#endif