#include <GL/glew.h>
#include <GL/gl.h>
#include "headers/Player.h"
#include <cmath>
#include <iostream>

#include "headers/Weapon.h"

#define M_PI 3.14159265358979323846
#define NUM_WEAPONS 9

bool Player::has_backpack = false;
int16_t Player::armor = 0;
int16_t Player::health = 100;
uint16_t Player::ammo[] = {24, 50, 60, 40};
uint16_t Player::max_ammo[] = {200, 100, 100, 600};
uint8_t Player::current_weapon = 3;
bool Player::has_weapon[] = {1,0,1,1,1,1,1,1,1};
bool Player::next_weapon_selected = false;
double Player::next_weapon_selected_timer = 0;

Player::Player(svec3 position, float angle, NewModels::Map* map) {
	this->position = position;
	pos = fvec3(position).xzy();
	this->camera = new Camera(angle);

	this->new_map = map;
	this->current_sector = map->getPlayerSector({position.x,position.y},nullptr);
	this->ray_launched = false;
}

void Player::HandleEvent() {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	float yaw_rad = camera->GetYaw() * M_PI / 180.0f;

	fvec3 forward(sinf(yaw_rad), 0.0f, cosf(yaw_rad));
	fvec3 right(cosf(yaw_rad), 0.0f, -sinf(yaw_rad));
	fvec3 vertical(0.0f, 1.0f, 0.0f);

	float move_forward = 0.0f, move_strafe = 0.0f, move_vertical = 0.0f;
	if (keys[SDL_SCANCODE_W]) move_forward += 1;
	if (keys[SDL_SCANCODE_S]) move_forward -= 1;
	if (keys[SDL_SCANCODE_A]) move_strafe -= 1;
	if (keys[SDL_SCANCODE_D]) move_strafe += 1;
	if (keys[SDL_SCANCODE_SPACE]) move_vertical += 1;
	if (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL]) move_vertical -= 1;
	if (keys[SDL_SCANCODE_E] || keys[SDL_SCANCODE_KP_ENTER]) ray_launched = true;

	for (int i = SDL_SCANCODE_1; i <= SDL_SCANCODE_9; i++) {
		if (keys[i]) {
			if (TryPickWeapon(i-SDL_SCANCODE_1) && i - SDL_SCANCODE_1 != current_weapon) {
				next_weapon_selected = true;
				next_weapon_selected_timer = 20.0;
			}
			break;
		}
	}

	movement_vector = forward * move_forward + right * move_strafe;
}

void Player::HandleEvent(SDL_Event* event, double deltaTime) {
	if (event->type == SDL_MOUSEWHEEL) {
		if (event->wheel.y > 0) { // scroll up
			uint8_t old_weapon = current_weapon;
			SelectNextWeapon();
			if (current_weapon != old_weapon) {
				next_weapon_selected = true;
				next_weapon_selected_timer = 20.0;
			}
		} else if (event->wheel.y < 0) { // scroll down
			uint8_t old_weapon = current_weapon;
			SelectPreviousWeapon();
			if (current_weapon != old_weapon) {
				next_weapon_selected = true;
				next_weapon_selected_timer = 20.0;
			}
		}
	}
	else {
		camera->HandleEvent(event, deltaTime);
	}
}

void Player::Update(double deltaTime) {
	float speed = 100.0f * deltaTime;
	//Update player's postion only once;
	movement_vector = movement_vector.normalized()*speed;
	new_map->HandleMovement(movement_vector, pos, current_sector);
	pos.y = current_sector->floor_height + 46;
	movement_vector = fvec3::zero;

	if (ray_launched) {
		auto cam_vec = camera->get3DVector();
		new_map->TryActivateRay(cam_vec, current_sector, pos);
	}

}

void Player::Render() {
	camera->Render();
	glTranslatef(-pos.x, -pos.y, pos.z);
}

void Player::SelectPreviousWeapon() {
	printf("Weapon %hhu selected", current_weapon);
	int i = current_weapon;
	do {
		i = (i + NUM_WEAPONS - 1) % NUM_WEAPONS;
	} while (!has_weapon[i]);
	current_weapon = i;
	printf("Weapon %hhu selected", current_weapon);
}

void Player::SelectNextWeapon() {
	int i = current_weapon;
	do {
		i = (i + 1) % NUM_WEAPONS;
	} while (!has_weapon[i]);
	current_weapon = i;
	printf("Weapon %hhu selected", current_weapon);
}

gl_texture Player::GetCurrentWeaponTexture() {

}

WeaponType Player::GetCurrentWeaponType() {
	return static_cast<WeaponType>(Player::current_weapon);
}

bool Player::TryPickWeapon(uint8_t weapon_index) {
	if (has_weapon[weapon_index]) {
		uint8_t old_weapon = current_weapon;
		current_weapon = weapon_index;
		if (current_weapon != old_weapon) {
			next_weapon_selected = true;
			next_weapon_selected_timer = 20.0;
		}
		return true;
	}
	return false;
}

uint8_t Player::GetCurrentAmmoType() {
	WeaponType t = (WeaponType)current_weapon;
	if (t == WeaponType::PISTOL || t == WeaponType::CHAINGUN)
		return 0;
	if (t == WeaponType::SHOTGUN || t == WeaponType::SUPER_SHOTGUN)
		return 1;
	if (t == WeaponType::ROCKET_LAUNCHER)
		return 2;
	if (t== WeaponType::BFG9000 || t == WeaponType::PLASMA_RIFLE)
		return 3;
	return 4;
}