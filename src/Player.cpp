#include "headers/Player.h"
#include <cmath>

#define M_PI 3.14159265358979323846

Player::Player(NewModels::vec3 position, float angle) {
	this->position = position;
	this->pos_x = static_cast<float>(position.x);
	this->pos_y = static_cast<float>(position.z);
	this->pos_z = static_cast<float>(position.y);
	this->camera = new Camera(angle);
	this->current_sector = nullptr;
	this->map = nullptr;
}

void Player::HandleEvent(SDL_Event* event, double deltaTime) {
	camera->HandleEvent(event, deltaTime);

	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	float speed = 40.0f * deltaTime; // Adjusted for Doom map scale

	float yaw_rad = camera->GetYaw() * M_PI / 180.0f;


	float forward_x = sinf(yaw_rad);
	float forward_z = cosf(yaw_rad);
	float right_x = cosf(yaw_rad);
	float right_z = -sinf(yaw_rad);

	float move_forward = 0.0f, move_strafe = 0.0f, move_vertical = 0.0f;
	if (keys[SDL_SCANCODE_W]) move_forward += speed;
	if (keys[SDL_SCANCODE_S]) move_forward -= speed;
	if (keys[SDL_SCANCODE_A]) move_strafe -= speed;
	if (keys[SDL_SCANCODE_D]) move_strafe += speed;
	if (keys[SDL_SCANCODE_SPACE]) move_vertical += speed;
	if (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL]) move_vertical -= speed;

	pos_x += fmin(forward_x * move_forward + right_x * move_strafe,speed);
	pos_z += fmin(forward_z * move_forward + right_z * move_strafe,speed);
	pos_y += move_vertical;

	// Update short position (optional, for map interactions)
	position.x = static_cast<short>(pos_x);
	position.y = static_cast<short>(pos_y);
	position.z = static_cast<short>(pos_z);


	// printf("Yaw : %lf\n", camera->GetYaw());
	// printf("Position : %lf %lf\n", pos_x, pos_z);
}

void Player::Render() {
	camera->Render();
	glTranslatef(-pos_x, -pos_y, pos_z);
}