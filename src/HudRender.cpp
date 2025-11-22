//
// Created by Wiktor on 19.11.2025.
//

#include "headers/HudRender.h"
#include "headers/game.h"
#include "headers/stb_image.h"

uint16_t HudRender::w = 0;
uint16_t HudRender::h = 0;
Numbers HudRender::numbers = {};
Icons HudRender::icons = {};
const std::string HudRender::hud_textures_path = "./textures/";

void HudRender::InvertColor(uint8_t *pixels, const int width, const int height) {
	auto *p = reinterpret_cast<color4 *>(pixels);
	for (int i = 0; i < width * height; i++) {
		if (p[i].r >150 && p[i].g >150 && p[i].b >150)
			p[i].a = 0;

		if (p[i].a == 0) continue;
		p[i].r = 255;
		p[i].g = 255;
		p[i].b = 255;
	}
}

gl_texture HudRender::LoadTexture(const char *im_path, const bool invert_color) {
	int width, height, channels;
	auto texture = stbi_load(im_path, &width, &height, &channels, 0);
	if (invert_color && texture)
		InvertColor(texture, width, height);

	if (texture) {
		GLuint texture_id;

		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		auto format = channels == 4 ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texture);
		stbi_image_free(texture);
		return gl_texture{static_cast<uint16_t>(width),static_cast<uint16_t>(height), texture_id};
	}
	return gl_texture{static_cast<uint16_t>(0),static_cast<uint16_t>(0), 0};
}

void HudRender::LoadNumber(uint8_t i, gl_texture &number) {
	std::string texture_name = hud_textures_path + "/numbers/256/number" + std::to_string(i)+"_256.png";
	number = LoadTexture(texture_name.c_str());
}

void HudRender::LoadIcon(std::string name, gl_texture &icon) {
	std::string texture_name = hud_textures_path + "icons/" + name +".png";
	icon = LoadTexture(texture_name.c_str(), true);
}

void HudRender::LoadHudTextures() {
	printf("Hud loading\n");
	LoadIcon("plus", HudRender::icons.plus_icon);
	LoadIcon("armor", HudRender::icons.armor_icon);

	LoadIcon("bullet", HudRender::icons.ammo_icons[0]);
	LoadIcon("shell", HudRender::icons.ammo_icons[1]);
	LoadIcon("rocket2", HudRender::icons.ammo_icons[2]);
	LoadIcon("cell", HudRender::icons.ammo_icons[3]);

	for (auto i = 0; i < 10; i++) {
		LoadNumber(i, HudRender::numbers.numbers[i]);
	}

	printf("Hud loaded\n");
}

void HudRender::DestroyHudTextures(){
	glDeleteTextures(1, &HudRender::icons.plus_icon.texture_id);
	glDeleteTextures(1, &HudRender::icons.armor_icon.texture_id);

	for (auto& ammo_tex : HudRender::icons.ammo_icons) {
		glDeleteTextures(1, &ammo_tex.texture_id);
	}

	for (auto & number : HudRender::numbers.numbers) {
		glDeleteTextures(1, &number.texture_id);
	}
}

void HudRender::Render() {
	Game::GetScreenSize(w, h);

	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, (GLfloat)w, 0, (GLfloat)h, -1.f, 1.f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	RenderHealth();
	RenderArmor();
	RenderAmmo();
	RenderGun();
	RenderCrosshair();

	glPopMatrix();                 // modelview
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();                 // projection

	glMatrixMode(GL_MODELVIEW);    // <- bardzo wazne
	glEnable(GL_DEPTH_TEST);
}

void HudRender::RenderHealth() {
    uint16_t h = HudRender::h * 0.1f;
    uint16_t w = HudRender::w * 0.15f;
    uint16_t x = HudRender::w *0.1f;

    if (h*3>w)
       w = h*3;
    else if (w>h*3)
       h = w/3.f;

    float margin_x = HudRender::w * 0.05f;
    float margin_y = HudRender::h * 0.05f;
    float health_alloc_width = HudRender::w * 0.15f;
    float s = std::min(static_cast<float>(HudRender::h) * 0.1f, health_alloc_width / 4.0f);

    float x_pos = margin_x;
    float y_pos = margin_y;

    uint8_t nums[3];
    uint8_t i = 0;
    int16_t player_health = Player::health <= 0 ? 0 : Player::health;

    while (player_health > 0) {
       nums[2-i] = player_health % 10;
       player_health /= 10;
       i++;
    }

    if (i == 0) {
        nums[2] = 0;
        i = 1;
    }

    float num_start_x = x_pos + s + (3.0f * s - static_cast<float>(i) * s);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw icon
    glBindTexture(GL_TEXTURE_2D, icons.plus_icon.texture_id);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(x_pos, y_pos);
    glTexCoord2f(1, 1); glVertex2f(x_pos + s, y_pos);
    glTexCoord2f(1, 0); glVertex2f(x_pos + s, y_pos + s);
    glTexCoord2f(0, 0); glVertex2f(x_pos, y_pos + s);
    glEnd();

    // Draw numbers
    for (uint8_t j = 3 - i; j < 3; j++) {
        glBindTexture(GL_TEXTURE_2D, numbers.numbers[nums[j]].texture_id);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex2f(num_start_x, y_pos);
        glTexCoord2f(1, 1); glVertex2f(num_start_x + s, y_pos);
        glTexCoord2f(1, 0); glVertex2f(num_start_x + s, y_pos + s);
        glTexCoord2f(0, 0); glVertex2f(num_start_x, y_pos + s);
        glEnd();
        num_start_x += s;
    }

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

void HudRender::RenderArmor() {
    float margin_x = HudRender::w * 0.05f;
    float margin_y = HudRender::h * 0.05f;
    float alloc_width = HudRender::w * 0.15f;
    float s = std::min(static_cast<float>(HudRender::h) * 0.1f, alloc_width / 4.0f);
    float health_total_width = 4.0f * s;
    float space = HudRender::w * 0.02f;

    float x_pos = margin_x + health_total_width + space;
    float y_pos = margin_y;

    uint8_t nums[3];
    uint8_t i = 0;
    int16_t player_armor = Player::armor <= 0 ? 0 : Player::armor;

    while (player_armor > 0) {
       nums[2-i] = player_armor % 10;
       player_armor /= 10;
       i++;
    }

    if (i == 0) {
        nums[2] = 0;
        i = 1;
    }

    float num_start_x = x_pos + s + (3.0f * s - static_cast<float>(i) * s);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw icon
    glBindTexture(GL_TEXTURE_2D, icons.armor_icon.texture_id);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(x_pos, y_pos);
    glTexCoord2f(1, 1); glVertex2f(x_pos + s, y_pos);
    glTexCoord2f(1, 0); glVertex2f(x_pos + s, y_pos + s);
    glTexCoord2f(0, 0); glVertex2f(x_pos, y_pos + s);
    glEnd();

    // Draw numbers
    for (uint8_t j = 3 - i; j < 3; j++) {
        glBindTexture(GL_TEXTURE_2D, numbers.numbers[nums[j]].texture_id);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex2f(num_start_x, y_pos);
        glTexCoord2f(1, 1); glVertex2f(num_start_x + s, y_pos);
        glTexCoord2f(1, 0); glVertex2f(num_start_x + s, y_pos + s);
        glTexCoord2f(0, 0); glVertex2f(num_start_x, y_pos + s);
        glEnd();
        num_start_x += s;
    }

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

void HudRender::RenderAmmo() {
	auto current_ammo = Player::GetCurrentAmmoType();
	if (current_ammo == 4)
		return;

	auto player_ammo = Player::ammo[current_ammo];

	float margin_x = HudRender::w * 0.05f;
	float margin_y = HudRender::h * 0.05f;
	float alloc_width = HudRender::w * 0.15f;
	float s = std::min(static_cast<float>(HudRender::h) * 0.1f, alloc_width / 4.0f);
	float total_width = 4.0f * s;

	float x_pos = HudRender::w - margin_x - total_width;
	float y_pos = margin_y;

	uint8_t nums[3];
	uint8_t i = 0;
	uint16_t ammo_count = player_ammo;

	while (ammo_count > 0) {
		nums[2-i] = ammo_count % 10;
		ammo_count /= 10;
		i++;
	}

	if (i == 0) {
		nums[2] = 0;
		i = 1;
	}

	float num_start_x = x_pos + s + (3.0f * s - static_cast<float>(i) * s);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw icon
	glBindTexture(GL_TEXTURE_2D, icons.ammo_icons[current_ammo].texture_id);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex2f(x_pos, y_pos);
	glTexCoord2f(1, 1); glVertex2f(x_pos + s, y_pos);
	glTexCoord2f(1, 0); glVertex2f(x_pos + s, y_pos + s);
	glTexCoord2f(0, 0); glVertex2f(x_pos, y_pos + s);
	glEnd();

	// Draw numbers
	for (uint8_t j = 3 - i; j < 3; j++) {
		glBindTexture(GL_TEXTURE_2D, numbers.numbers[nums[j]].texture_id);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 1); glVertex2f(num_start_x, y_pos);
		glTexCoord2f(1, 1); glVertex2f(num_start_x + s, y_pos);
		glTexCoord2f(1, 0); glVertex2f(num_start_x + s, y_pos + s);
		glTexCoord2f(0, 0); glVertex2f(num_start_x, y_pos + s);
		glEnd();
		num_start_x += s;
	}

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

const int wep_mul = 2;
void HudRender::RenderGun() {
	gl_texture WeaponTexture;
	auto res = Player::GetCurrentWeaponFrame(WeaponTexture);
	if (!res)
		return;

	gl_texture FlashTexture;
	auto res2 = Player::GetCurrentFlashFrame(FlashTexture);

	auto gunX = (w - WeaponTexture.w * wep_mul) / 2.;
	auto gunW = WeaponTexture.w * wep_mul;
	auto gunH = WeaponTexture.h * wep_mul;

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, WeaponTexture.texture_id);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex2f(gunX, 0);
	glTexCoord2f(1, 1); glVertex2f(gunX + gunW, 0);
	glTexCoord2f(1, 0); glVertex2f(gunX + gunW, gunH);
	glTexCoord2f(0, 0); glVertex2f(gunX, gunH);
	glEnd();

	if (!res2)
	{
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		return;
	}

	auto offsets = Player::GetCurrentFlashOffset();

	auto scaled_left_offset = offsets.x * wep_mul;
	auto scaled_top_offset = offsets.y * wep_mul;
	auto flashW = FlashTexture.w * wep_mul;
	auto flashH = FlashTexture.h * wep_mul;

	auto flashX = (w / 2.) - scaled_left_offset;
	auto flash_bottom_y = scaled_top_offset - flashH;
	auto flash_top_y = scaled_top_offset;

	glBindTexture(GL_TEXTURE_2D, FlashTexture.texture_id);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex2f(flashX, flash_bottom_y);
	glTexCoord2f(1, 1); glVertex2f(flashX + flashW, flash_bottom_y);
	glTexCoord2f(1, 0); glVertex2f(flashX + flashW, flash_top_y);
	glTexCoord2f(0, 0); glVertex2f(flashX, flash_top_y);
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

void HudRender::RenderCrosshair()
{
	const float crosshairSize = 16.0f;        // total size in pixels (adjust to taste)
	const float thickness     = 2.0f;         // thickness of lines
	const float gap           = 4.0f;         // empty space in the center

	const float halfSize = crosshairSize * 0.5f;
	const float halfThick = thickness * 0.5f;

	float centerX = w * 0.5f;
	float centerY = h * 0.5f;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bright green crosshair (classic FPS style)
	glColor4f(0.0f, 1.0f, 0.0f, 0.8f);

	glBegin(GL_QUADS);

	// Horizontal line
	glVertex2f(centerX - halfSize,           centerY - halfThick);
	glVertex2f(centerX - gap,                centerY - halfThick);
	glVertex2f(centerX - gap,                centerY + halfThick);
	glVertex2f(centerX - halfSize,           centerY + halfThick);

	glVertex2f(centerX + gap,                centerY - halfThick);
	glVertex2f(centerX + halfSize,           centerY - halfThick);
	glVertex2f(centerX + halfSize,           centerY + halfThick);
	glVertex2f(centerX + gap,                centerY + halfThick);

	// Vertical line
	glVertex2f(centerX - halfThick,          centerY - halfSize);
	glVertex2f(centerX + halfThick,          centerY - halfSize);
	glVertex2f(centerX + halfThick,          centerY - gap);
	glVertex2f(centerX - halfThick,          centerY - gap);

	glVertex2f(centerX - halfThick,          centerY + gap);
	glVertex2f(centerX + halfThick,          centerY + gap);
	glVertex2f(centerX + halfThick,          centerY + halfSize);
	glVertex2f(centerX - halfThick,          centerY + halfSize);

	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  // reset color
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);  // restore default state if needed
}