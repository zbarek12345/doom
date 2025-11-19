//
// Created by Wiktor on 19.11.2025.
//

#ifndef DOOM_HUDRENDER_H
#define DOOM_HUDRENDER_H
#include <string>

#include "texture.h"

struct Numbers {
	gl_texture numbers[10];
};

struct Icons {
	gl_texture plus_icon;
	gl_texture armor_icon;
	gl_texture ammo_icons[4];
};

class HudRender {
	static const std::string hud_textures_path;

	static Numbers numbers;
	static Icons icons;
	static uint16_t w, h;

	static void InvertColor(uint8_t *pixels, int width, int height);

	static gl_texture LoadTexture(const char *im_path, bool invert_color = false);
	static void LoadNumber(uint8_t i, gl_texture& number);
	static void LoadIcon(std::string name, gl_texture& icon);

	static void RenderHealth();
	static void RenderArmor();
	static void RenderAmmo();
	static void RenderGun();

public:
	static void LoadHudTextures();

	static void DestroyHudTextures();

	static void Render();


};

#endif //DOOM_HUDRENDER_H