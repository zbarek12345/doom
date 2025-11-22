//
// Created by Wiktor on 22.11.2025.
//

#include "headers/Projectile.h"

uint16_t Projectile::GetDamage() const {
	return rand()%(max_damage - min_damage) + min_damage;
}

ProjectileType Projectile::GetType() const {
	return type;
}


BulletProjectile::
