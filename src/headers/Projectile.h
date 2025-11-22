//
// Created by Wiktor on 19.11.2025.
//

#ifndef DOOM_PROJECTILE_H
#define DOOM_PROJECTILE_H
#include <cstdint>
#include <iostream>

#include "vec3.h"

enum class ProjectileType {
	PUNCH,       // Short-range melee projectile for fist
	SAW,         // Short-range melee projectile for chainsaw
	BULLET,      // For pistol/chaingun
	PELLET,      // For shotgun
	ROCKET,      // For rocket launcher
	PLASMA,      // For plasma rifle
	BFG_BALL     // For BFG9000
};

class Projectile {
public:
	virtual ~Projectile() = default;

	Projectile(ProjectileType type, const fvec3& position, const fvec3& direction,
						uint16_t speed, uint16_t min_damage, uint16_t max_damage) {
		this->position = position;
		this->direction = direction;
		this->type = type;
	}

private:
	ProjectileType type;
	uint16_t speed = 0;
	uint16_t min_damage=0;
	uint16_t max_damage=0;

	fvec3 direction;
	fvec3 position;

	ProjectileType GetType() const;
	uint16_t GetDamage() const;
	virtual bool HasExplosion(uint16_t& damage, uint16_t& radius);
	virtual void Update(double deltaTime);
	virtual void Render();
};

class BulletProjectile : public Projectile {
	BulletProjectile(fvec3 position, fvec3 direction):Projectile(ProjectileType::BULLET, position, direction, 600, 5, 15){}

	bool HasExplosion(uint16_t &damage, uint16_t &radius) override;
	void Update(double deltaTime) override;
	void Render() override;
};

#endif //DOOM_PROJECTILE_H