//
// Created by Wiktor on 19.11.2025.
//

#ifndef DOOM_WEAPON_H
#define DOOM_WEAPON_H
#include <random>
#define M_PI 3.14159265358979323846
#include "Projectile.h"

enum class WeaponType {
    FIST,
    CHAINSAW,
    PISTOL,
    SHOTGUN,
    CHAINGUN,
    ROCKET_LAUNCHER,
    PLASMA_RIFLE,
    BFG9000,
    SUPER_SHOTGUN
};

enum class WeaponState {
	READY,      // Weapon is idle and ready to fire
	FIRING,     // In firing animation (projectile spawn happens here)
	RELOADING,  // Post-fire cooldown/reload
	LOWERING,   // Switching away
	RAISING     // Switching to
};

class DoomGunInterface {
private:
    WeaponType currentType;
    WeaponState currentState;
    int cooldownTicks;  // Simple timer for state transitions (e.g., fire rate)
    std::mt19937 rng;   // For random shotgun spread

    // Helper to transition states
    void SetState(WeaponState newState) {
        currentState = newState;
        // Reset timers or other logic as needed
        switch (newState) {
            case WeaponState::FIRING:
                cooldownTicks = GetFireDelay(currentType);  // Example delays
                break;
            case WeaponState::READY:
                cooldownTicks = 0;
                break;
            default:
                break;
        }
    }

    // Get fire delay in ticks (arbitrary values mimicking original fire rates)
    int GetFireDelay(WeaponType type) const {
        switch (type) {
            case WeaponType::FIST: return 10;
            case WeaponType::CHAINSAW: return 5;
            case WeaponType::PISTOL: return 15;
            case WeaponType::SHOTGUN: return 30;
            case WeaponType::CHAINGUN: return 5;
            case WeaponType::ROCKET_LAUNCHER: return 40;
            case WeaponType::PLASMA_RIFLE: return 10;
            case WeaponType::BFG9000: return 50;
            default: return 20;
        }
    }

public:
    DoomGunInterface(WeaponType initialType = WeaponType::PISTOL)
        : currentType(initialType), currentState(WeaponState::READY),
          cooldownTicks(0), rng(std::random_device{}()) {}

    virtual ~DoomGunInterface() = default;

    // Switch to a different weapon type
    void SetWeaponType(WeaponType type) {
        currentType = type;
        SetState(WeaponState::RAISING);  // Simulate raise animation
        // In full impl, transition to READY after raise
        SetState(WeaponState::READY);
    }

    // Update the weapon state each frame/tick
    void Update() {
        if (cooldownTicks > 0) {
            --cooldownTicks;
            if (cooldownTicks == 0) {
                if (currentState == WeaponState::FIRING) {
                    SetState(WeaponState::RELOADING);
                } else if (currentState == WeaponState::RELOADING) {
                    SetState(WeaponState::READY);
                }
            }
        }
    }

    // Fire the weapon, spawning projectile(s) of the specified type
    // Assert: All weapons spawn projectiles (melee as short-range proj)
    void Fire(const fvec3& position, const fvec3& target) {
        if (currentState != WeaponState::READY) return;  // Can't fire yet

        fvec3 direction = (target - position).normalized();
        SetState(WeaponState::FIRING);

        // Spawn based on type
        switch (currentType) {
            case WeaponType::FIST:
                new Projectile(ProjectileType::PUNCH, position, direction);
                break;
            case WeaponType::CHAINSAW:
                new Projectile(ProjectileType::SAW, position, direction);
                break;
            case WeaponType::PISTOL:
                new Projectile(ProjectileType::BULLET, position, direction);
                break;
            case WeaponType::SHOTGUN:
                // Spawn 7 pellets with random spread (mimicking original spread)
                for (int i = 0; i < 7; ++i) {
                    // Horizontal spread ~0-10 degrees, vertical ~0-5 degrees (approx)
                    std::uniform_real_distribution<float> distHorizontal(-10.0f, 10.0f);
                    std::uniform_real_distribution<float> distVertical(-5.0f, 5.0f);
                    float angleH = distHorizontal(rng) * (static_cast<float>(M_PI) / 180.0f);
                    float angleV = distVertical(rng) * (static_cast<float>(M_PI) / 180.0f);

                    // Apply rotation to direction (assuming z-forward, x-right, y-up)
                    fvec3 spreadDir;
                    // Rotate horizontal (around y-axis)
                    spreadDir.x = direction.x * std::cos(angleH) + direction.z * std::sin(angleH);
                    spreadDir.z = -direction.x * std::sin(angleH) + direction.z * std::cos(angleH);
                    spreadDir.y = direction.y;
                    // Rotate vertical (around x-axis)
                    float tempY = spreadDir.y * std::cos(angleV) - spreadDir.z * std::sin(angleV);
                    spreadDir.z = spreadDir.y * std::sin(angleV) + spreadDir.z * std::cos(angleV);
                    spreadDir.y = tempY;

                    new Projectile(ProjectileType::PELLET, position, spreadDir.normalized());
                }
                break;
            case WeaponType::CHAINGUN:
                new Projectile(ProjectileType::BULLET, position, direction);
                break;
            case WeaponType::ROCKET_LAUNCHER:
                new Projectile(ProjectileType::ROCKET, position, direction);
                break;
            case WeaponType::PLASMA_RIFLE:
                new Projectile(ProjectileType::PLASMA, position, direction);
                break;
            case WeaponType::BFG9000:
                new Projectile(ProjectileType::BFG_BALL, position, direction);
                break;
        }
    }
};
#endif //DOOM_WEAPON_H