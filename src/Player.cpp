#include <GL/glew.h>
#include <GL/gl.h>
#include "headers/Player.h"
#include <cmath>
#include <iostream>

#define M_PI 3.14159265358979323846

bool Player::has_backpack = false;
int16_t Player::armor = 0;
int16_t Player::health = 100;
uint16_t Player::ammo[] = {0, 0, 0, 0};
uint16_t Player::max_ammo[] = {200, 100, 100, 600};
bool Player::has_weapon[] = {1,1,0,0,0,0,0,0,0};

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

	movement_vector = forward * move_forward + right * move_strafe;
}

void Player::HandleEvent(SDL_Event* event, double deltaTime) {
	camera->HandleEvent(event, deltaTime);
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
