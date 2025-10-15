//
// Created by Wiktor on 11.10.2025.
//
#include <unordered_map>
#include <string>
#include <cstdint>
class TexBinder {
   std::unordered_map<std::string, uint32_t> textures;
   std::string base_path = R"(C:\Users\Wiktor\Downloads\GZDoom_HD_Texture_pack.6\GZDoom\Mods\hires)";

public:
   void LoadTexture(std::string path);

   uint32_t GetTexture(const char* texture_name);

   ~TexBinder();
};