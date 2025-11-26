//
// Created by Wiktor on 11.10.2025.
//
#ifndef TEXBINDER_H
#define TEXBINDER_H

#include <unordered_map>
#include <string>
#include <cstdint>

#include "OriginalTextureRenderer.h"
#include "RawLumpKeeper.h"

enum class TextureType {
   FlatTexture,
   WallTexture,
   ObstacleTexture,
   ItemTexture,
   PlayerTexture,
   WeaponTexture,
   MonsterTexture
};

class TexBinder {
   std::unordered_map<std::string, gl_texture> textures;
   std::string base_path = R"(C:\Users\Wiktor\Downloads\GZDoom_HD_Texture_pack.6\GZDoom\Mods\hires)";
   OriginalTextureRenderer*original_texture_renderer = nullptr;

public:

   void BindOriginalTextureRenderer(OriginalTextureRenderer* original_texture_renderer);

   void LoadTexture(std::string path, TextureType type = TextureType::FlatTexture);

   gl_texture GetTexture(const char* texture_name, TextureType type = TextureType::FlatTexture);

   ~TexBinder();
};

#endif //TEXBINDER_H