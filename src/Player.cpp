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
uint16_t Player::ammo[] = {50, 0, 0, 0};
uint16_t Player::max_ammo[] = {200, 100, 100, 600};
uint8_t Player::current_weapon = 2;
bool Player::has_weapon[] = {1,0,1,0,0,0,0,0,0};
bool Player::next_weapon_selected = false;
double Player::next_weapon_selected_timer = 0;
std::vector<DoomGunInterface*> Player::weapons={};
svec3 Player::position = {0,0,0};
fvec3 Player::pos = {0,0,0};
fvec3 Player::LookDir = {0,0,0};
Player* Player::Instance = nullptr;
bool Player::keys[3] = {0,0,0};

Player::Player(svec3 position, float angle, NewModels::Map* map) {
	Player::position = position;
	pos = (fvec3)position;
	pos = fvec3(position).xzy();
	this->camera = new Camera(angle);

	this->new_map = map;
	this->current_sector = map->getPlayerSector({position.x,position.y},nullptr);
	this->ray_launched = false;
	Instance = this;
}

void Player::HandleEvent() {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	float yaw_rad = camera->GetYaw() * M_PI / 180.0f;

	fvec3 forward(sinf(yaw_rad), 0.0f, cosf(yaw_rad));
	fvec3 right(cosf(yaw_rad), 0.0f, -sinf(yaw_rad));
	fvec3 vertical(0.0f, 1.0f, 0.0f);

	float move_forward = 0.0f, move_strafe = 0.0f;
	if (keys[SDL_SCANCODE_W]) move_forward += 1;
	if (keys[SDL_SCANCODE_S]) move_forward -= 1;
	if (keys[SDL_SCANCODE_A]) move_strafe -= 1;
	if (keys[SDL_SCANCODE_D]) move_strafe += 1;
	if (keys[SDL_SCANCODE_E] || keys[SDL_SCANCODE_SPACE]) ray_launched = true;

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
	float speed = 350.0f * deltaTime;
	//Update player's postion only once;
	movement_vector = movement_vector.normalized()*speed;
	new_map->HandleMovement(movement_vector, pos, current_sector);
	pos.y = current_sector->floor_height + 46;
	movement_vector = fvec3::zero;

	if (ray_launched) {
		auto cam_vec = camera->get3DVector();
		new_map->TryActivateRay(cam_vec, current_sector, pos);
		ray_launched = false;
	}
	weapons[current_weapon]->Update(deltaTime);

	LookDir = camera->get3DVector();
}

void Player::Render() {
	camera->Render();
	glTranslatef(-pos.x, -pos.y, pos.z);
}

bool Player::CanChangeWeapon() {
	if (weapons[current_weapon]==nullptr)
		return true;
	return !weapons[current_weapon]->lockChange();
}

void Player::SelectPreviousWeapon() {
	if (!Player::CanChangeWeapon())
		return;
	printf("Weapon %hhu selected", current_weapon);
	int i = current_weapon;
	do {
		i = (i + NUM_WEAPONS - 1) % NUM_WEAPONS;
	} while (!has_weapon[i]);
	current_weapon = i;
	if (weapons[current_weapon])
		weapons[current_weapon]->Select();
	printf("Weapon %hhu selected", current_weapon);
}

void Player::SelectNextWeapon() {
	if (!Player::CanChangeWeapon())
		return;
	int i = current_weapon;
	do {
		i = (i + 1) % NUM_WEAPONS;
	} while (!has_weapon[i]);
	current_weapon = i;
	if (weapons[current_weapon])
		weapons[current_weapon]->Select();
	printf("Weapon %hhu selected", current_weapon);
}

bool Player::GetCurrentFlashFrame(gl_texture &frame) {
	if (weapons[current_weapon] != nullptr)
		return weapons[current_weapon]->GetCurrentFlashFrame(frame);
	return false;
}

svec2 Player::GetCurrentFlashOffset() {
	return weapons[current_weapon]->GetFlashOffset();
}

bool Player::GetCurrentWeaponFrame(gl_texture &frame) {
	if (weapons[current_weapon] != nullptr)
		return weapons[current_weapon]->GetCurrentFrame(frame);
	return false;
}

WeaponType Player::GetCurrentWeaponType() {
	return static_cast<WeaponType>(Player::current_weapon);
}

bool Player::TryPickWeapon(uint8_t weapon_index) {
	if (CanChangeWeapon())
		return false;
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

void Player::TryShoot() {
	if (weapons[current_weapon]!=nullptr) {
		auto at = GetCurrentAmmoType();
		if (at == 4)
			weapons[current_weapon]->TryShot();
		else if (Player::ammo[at] > 0)
			ammo[at]-=weapons[current_weapon]->TryShot();
	}
}

void Player::TakeDamage(uint16_t dmg) {
	if (health <= 0) return;

	int remaining = (int)dmg;

	//prosta absorpcja przez armor, cos w stylu dooma
	if (armor > 0 && remaining > 0) {
		int armor_absorb = remaining * 2 / 3; //2/3 idzie w armor
		if (armor_absorb > armor) armor_absorb = armor;
		armor -= armor_absorb;
		remaining -= armor_absorb;
	}

	if (remaining <= 0) return;

	health -= remaining;
	if (health < 0) health = 0;
	//tu ewentualnie: flaga 'player hurt', dzwiek, efekt kamery itd.
}

int16_t Player::GetHealth() {
	return health;
}

void Player::BindWeapons(std::vector<DoomGunInterface *> weapons) {
	assert(weapons.size() == NUM_WEAPONS);
	Player::weapons = weapons;
	weapons[current_weapon]->Select();
}

fvec3 Player::GetPosition() {
	return pos;
}

Player* Player::GetInstance() {
	return Instance;
}
