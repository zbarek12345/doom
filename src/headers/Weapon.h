//
// Created by Wiktor on 19.11.2025.
//

#ifndef DOOM_WEAPON_H
#define DOOM_WEAPON_H
#include <random>
#define M_PI 3.14159265358979323846
#include "Projectile.h"
#include "texture.h"
#include "vec2.h"

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
	REFIRING,  // Post-fire cooldown/reload
    PICK_UP,
};

struct animationFrameLoader {
    char let;
    float time;
};

struct animationFrame {
    gl_texture texture;
    double time;
};

struct DoomGunInitiator {
    std::string base_texture_name;
    std::string flash_texture_name;
    double pickupDelay = 10.;
    std::vector<std::vector<animationFrameLoader>> loaders;
    std::vector<animationFrameLoader> flashLoader;

    DoomGunInitiator(std::string base_texture_name, std::string flash_texture_name, double pickupDelay, std::vector<std::vector<animationFrameLoader>> loaders, std::vector<animationFrameLoader> flashLoader) : base_texture_name(base_texture_name), flash_texture_name(flash_texture_name), pickupDelay(pickupDelay), loaders(loaders), flashLoader(flashLoader) {}
};

class DoomGunInterface {
private:
    WeaponState currentState = WeaponState::READY;
    double pickupDelay = .04;
    double pickupTimer = 0.;
    double lastShotTime = 0.;


    double flashTimer = 0.;
    int16_t currentFlashFrame = -1;
    std::vector<animationFrame> frames[(int)WeaponState::PICK_UP] = {};
    std::vector<animationFrame> flash = {};
    std::vector<svec2> flashOffsets;
    double frameTimer = 0.;
    size_t currentFrame = 0;
public:
    DoomGunInterface() = default;

    void SetDelay(double delay) {
        pickupDelay = delay;
    }

    void SetAnimationFrames(std::vector<animationFrame> frames[]) {
        for (int i =0;i<3;i++) {
            this->frames[i] = frames[i];
        }

    }

    void SetFlashFrames(std::vector<animationFrame> flash) {
        this->flash = flash;
    }

    void SetFlashOffsets(std::vector<svec2> offsets) {
        this->flashOffsets = offsets;
    }

    void Unselect() {
        currentState = WeaponState::PICK_UP;
    }

    void Select() {
        currentState = WeaponState::PICK_UP;
        pickupTimer = 0.;
    }

    void Update(double deltaTime) {
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

    bool TryShot(){
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
            return true;
        }
        return false;
    }

    bool GetCurrentFlashFrame(gl_texture& frame) const {
        if (currentFlashFrame == -1 || flash.size() == 0)
            return false;

        frame = flash[currentFlashFrame].texture;
        return true;
    }

    svec2 GetFlashOffset() const {
        return flashOffsets[currentFlashFrame];
    }

    bool GetCurrentFrame(gl_texture& frame) {
        if (currentState == WeaponState::PICK_UP)
            return false;
        frame = frames[static_cast<int>(currentState)][currentFrame].texture;
        return true;
    }

    bool lockChange() const {
        return !(currentState == WeaponState::READY || currentState ==  WeaponState::PICK_UP);
    }
};

const DoomGunInitiator Pistol = DoomGunInitiator{
    "PISG",
    "PISF",
    0.4,
    {
            { {'A', 10} },
            { {'B', 1}, {'C', 1}, {'B', 5}, {'A', 1} },
            { {'C', 1}, {'B', 5}, {'A', 1} }
    },
    { {'A', 1} }
};

const DoomGunInitiator Shotgun = DoomGunInitiator{
    "SHTG",
    "SHTF",
    0.6,
    {
            { {'A', 10} },  // Pickup/wield (matched to pistol for consistency)
            { {'A', 10}, {'B', 5}, {'C', 5}, {'D', 4}, {'C', 5}, {'B', 5}, {'A', 10} },  // Fire (A10 B5 C5 D4 C5 B5 A10; total ~44 tics)
            { {'A', 10}, {'B', 5}, {'C', 5}, {'D', 4}, {'C', 5}, {'B', 5}, {'A', 10} }   // Refire (same as fire)
    },
    { {'A', 7} }  // Flash (averaged duration; appears during early fire frames)
};

const DoomGunInitiator ChainGun = DoomGunInitiator{
        "CHGG",
        "CHGF",
    0.5,
{
                { {'A', 10} },  // Pickup/wield (matched to pistol for consistency)
                { {'A', 1}, {'B', 4},{'A', 1} },  // Fire (A10 B5 C5 D4 C5 B5 A10; total ~44 tics)
                {{'A', 1},{'B', 4},  }   // Refire (same as fire)
            },
{ {'A', 4} }
};

const DoomGunInitiator Fists = DoomGunInitiator{
        "PUNG",
         "",
         0.2,
        {
            {{'A',10}},
            {{'B',4}, {'C', 4}, {'D',5}, {'C',4}, {'B', 5}},
            {{'B',4}, {'C', 4}, {'D',5}, {'C',4}}
        },
    {}
};
#endif //DOOM_WEAPON_H