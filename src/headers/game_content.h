#ifndef GAME_CONTENT_H
#define GAME_CONTENT_H

#include "map.h"

///struct created in order to upkeep the wad file within the running game. Should take around 5mB so no trouble
struct game_content{
   ///TODO - Textures;

   std::vector<original_map*> maps;
};
#endif //GAME_CONTENT_H
