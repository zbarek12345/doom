#ifndef GAME_CONTENT_H
#define GAME_CONTENT_H

#include <unordered_map>
#include "map.h"

///Struct created in order to upkeep the wad file within the running game. Should take around 5mB so no trouble
struct game_content{
   ///TODO - Textures;
   std::vector<original_classes::palette*> color_palette;
   std::vector<original_classes::colormap*> color_map;
   original_classes::p_names p_names;
   std::unordered_map<std::string, original_classes::patch> patch;

   ///Storing the original map from WAD
   std::vector<original_map*> maps;
};
#endif //GAME_CONTENT_H
