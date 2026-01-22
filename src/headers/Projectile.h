//
// Created by Wiktor on 19.11.2025.
//

#ifndef DOOM_PROJECTILE_H
#define DOOM_PROJECTILE_H
#include "Sector.h"
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
						uint16_t speed, uint16_t min_damage, uint16_t max_damage, void* caster);

protected:
	NewModels::Sector* current_sector = nullptr;
	ProjectileType type;
	uint16_t speed = 0;
	uint16_t min_damage=0;
	uint16_t max_damage=0;
	void* caster = nullptr;
	fvec3 direction;
	fvec3 position;

public:
	ProjectileType GetType() const;
	uint16_t GetDamage() const;
	NewModels::Sector* GetSector() const;
	bool GetDetails(fvec3 &direction, fvec3 &position) const;
	void SetSector(NewModels::Sector* sector);
	void SetPosition(fvec3 position);
	void* GetCaster() const;

	void Update(double deltaTime);

	virtual bool HasExplosion(uint16_t& damage, uint16_t& radius);
	virtual void Render() = 0 ;
};

class BulletProjectile : public Projectile {
public:
	BulletProjectile(fvec3 position, fvec3 direction, void* Caster);
	bool HasExplosion(uint16_t &damage, uint16_t &radius) override;
	void Render() override;
};

#endif //DOOM_PROJECTILE_H