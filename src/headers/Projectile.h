//
// Created by Wiktor on 19.11.2025.
//

#ifndef DOOM_PROJECTILE_H
#define DOOM_PROJECTILE_H
#include <cstdint>

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
	uint16_t speed = 0;
	uint16_t min_damage=0;
	uint16_t max_damage=0;

	fvec3 direction;
	fvec3 position;
	Projectile(ProjectileType type, const fvec3& position, const fvec3& direction) {
		// In a real engine, this would initialize velocity, collision, etc.
		// For demo, just print
		std::cout << "Spawned projectile of type " << static_cast<int>(type)
				  << " at (" << position.x << ", " << position.y << ", " << position.z << ")"
				  << " direction (" << direction.x << ", " << direction.y << ", " << direction.z << ")\n";
	}

	void ApplyDamage();

	virtual void Update(double deltaTime);
	virtual void Render();
};

#endif //DOOM_PROJECTILE_H