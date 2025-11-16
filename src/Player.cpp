#include "headers/Player.h"
#include <cmath>

#define M_PI 3.14159265358979323846

Player::Player(svec3 position, float angle, NewModels::Map* map) {
	this->position = position;
	pos = fvec3(position).xzy();
	this->camera = new Camera(angle);

	this->new_map = map;
	this->current_sector = map->getPlayerSector({position.x,position.y},nullptr);
}

void Player::HandleEvent(SDL_Event* event, double deltaTime) {
	camera->HandleEvent(event, deltaTime);

	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	float speed = 50.0f * deltaTime; // Adjusted for Doom map scale

	float yaw_rad = camera->GetYaw() * M_PI / 180.0f;


	fvec3 forward(sinf(yaw_rad), 0.0f, cosf(yaw_rad));
	fvec3 right(cosf(yaw_rad), 0.0f, -sinf(yaw_rad));
	fvec3 vertical(0.0f, 1.0f, 0.0f);

	float move_forward = 0.0f, move_strafe = 0.0f, move_vertical = 0.0f;
	if (keys[SDL_SCANCODE_W]) move_forward += speed;
	if (keys[SDL_SCANCODE_S]) move_forward -= speed;
	if (keys[SDL_SCANCODE_A]) move_strafe -= speed;
	if (keys[SDL_SCANCODE_D]) move_strafe += speed;
	if (keys[SDL_SCANCODE_SPACE]) move_vertical += speed;
	if (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL]) move_vertical -= speed;


	movement_vector = movement_vector + ( forward * move_forward + right * move_strafe);


}

void Player::Update(double deltaTime) {
	float speed = 50.0f * deltaTime;
	//Update player's postion only once;
	movement_vector = movement_vector.normalized()*(200.*deltaTime);
	new_map->HandleMovement(movement_vector, pos, current_sector);
	pos.y = current_sector->floor_height + 46;
	movement_vector = fvec3::zero;
}

void Player::Render() {
	camera->Render();
	glTranslatef(-pos.x, -pos.y, pos.z);
}