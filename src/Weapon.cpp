//
// Created by Wiktor on 22.11.2025.
//
#include "headers/Weapon.h"

#include "headers/Map.h"
#include "headers/Player.h"

void DoomGunInterface::SetDelay(double delay) {
	pickupDelay = delay;
}

void DoomGunInterface::SetAnimationFrames(std::vector<animationFrame> frames[]) {
	for (int i =0;i<3;i++) {
		this->frames[i] = frames[i];
	}

}

void DoomGunInterface::SetFlashFrames(std::vector<animationFrame> flash) {
	this->flash = flash;
}

void DoomGunInterface::SetFlashOffsets(std::vector<svec2> offsets) {
	this->flashOffsets = offsets;
}

void DoomGunInterface::Unselect() {
	currentState = WeaponState::PICK_UP;
}

void DoomGunInterface::Select() {
	currentState = WeaponState::PICK_UP;
	pickupTimer = 0.;
}

void DoomGunInterface::Update(double deltaTime) {
	printf("%lf\n", deltaTime);
	if (currentState == WeaponState::FIRING || currentState == WeaponState::REFIRING) {
		frameTimer += deltaTime;
		if (frameTimer > frames[static_cast<int>(currentState)][currentFrame].time) {
			currentFrame++;
			frameTimer = 0.;
			if (currentFrame > frames[static_cast<int>(currentState)].size()-1) {
				currentState = WeaponState::READY;
				currentFrame = 0;
				lastShotTime = 0.;
			}
		}

		flashTimer += deltaTime;
		if (!flash.empty()) {
			if (currentFlashFrame != -1 && flashTimer > flash[currentFlashFrame].time) {
				currentFlashFrame++;
				flashTimer = 0.;
				if (currentFlashFrame >= flash.size()) currentFlashFrame = -1;
			}
		}


	}

	if (currentState == WeaponState::PICK_UP) {
		pickupTimer += deltaTime;
		if (pickupTimer>pickupDelay) {
			printf("Weapon picked up\n");
			currentState = WeaponState::READY;
		}
	}

	if (currentState == WeaponState::READY) {
		lastShotTime += deltaTime;
		if (frameTimer > frames[static_cast<int>(currentState)][currentFrame].time) {
			currentFrame = (currentFrame + 1 )% frames[static_cast<int>(currentState)].size();
		}
	}
}

bool DoomGunInterface::TryShot() {
	if (currentState == WeaponState::READY) {
		frameTimer = 0.;
		currentFrame = 0;
		if (frameTimer > 5) {
			currentState = WeaponState::FIRING;
			currentFlashFrame = 0;
			flashTimer = 0.;
		}
		else {
			currentState = WeaponState::REFIRING;
			currentFlashFrame = 0;
			flashTimer = 0.;
		}
		fvec3 start_bullet = Player::GetPosition() + Player::LookDir * 16.2;
		NewModels::Map::projectiles.emplace(new BulletProjectile(start_bullet, Player::LookDir));
		printf("Projectile Created");

		return true;
	}
	return false;
}

bool DoomGunInterface::GetCurrentFlashFrame(gl_texture &frame) const {
	if (currentFlashFrame == -1 || flash.size() == 0)
		return false;

	frame = flash[currentFlashFrame].texture;
	return true;
}

svec2 DoomGunInterface::GetFlashOffset() const {
	return flashOffsets[currentFlashFrame];
}

bool DoomGunInterface::GetCurrentFrame(gl_texture &frame) {
	if (currentState == WeaponState::PICK_UP)
		return false;
	frame = frames[static_cast<int>(currentState)][currentFrame].texture;
	return true;
}

bool DoomGunInterface::lockChange() const {
	return !(currentState == WeaponState::READY || currentState ==  WeaponState::PICK_UP);
}
