//
// Created by Wiktor on 02.10.2025.
//
#include "map.h"
#include "OriginalTextureRenderer.h"
#ifndef GAME_CONTENT_H
#define GAME_CONTENT_H

///struct created in order to upkeep the wad file within the running game. Should take around 5mB so no trouble
struct game_content{
   ///TODO - Textures;
   RawLumpKeeper raw_lump_keeper;
   OriginalTextureRenderer original_texture_renderer;
   std::vector<original_map*> maps;
};
#endif //GAME_CONTENT_H
