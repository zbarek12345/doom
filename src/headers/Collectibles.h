//
// Created by Wiktor on 18.11.2025.
//

#ifndef DOOM_COLLECTIBLES_H
#define DOOM_COLLECTIBLES_H
#include "AnimatedEntity.h"
#include "Entity.h"

enum class CollectibleType {
	None,
		///Bfg gun, contains additional 40 cells
		BFUG,
		///Chaingun, contains 20 additional bullets.
		MGUN,
		///Chainsaw,
		CSAW,
		///Plasma Gun, contains 20 ammo units
		PLAS,
		///Rocket launcher, contains 2 rockets
		LAUN,
		///Shotgun, contains 8 shells
		SHOT,
		///Shotgun, contains 8 shells
		SGN2,
		///4 shells for shotgun
		SHEL,
		///5 rockets
		BROK,
		///A pack of 20 shot shells
		SBOX,
		///A pack of 50 bullets for pistol/chain
		AMMO,
		///A clip of 10 bullets for pistol/chain
		CLIP,
		///A pack of 20 bullets for plasma/bfg
		CELL,
		///A pack of 100 bullets for plasma/bfg
		CELP,
		///A single rocket
		ROCK,
		///Refreshes player armor up to 100%
		ARM1,
		///Increases Ammo cap twice (adds some additional ammo (must update)).
		BPAK,
		///Increases health by 25% up to 100%
		MEDI,
		///Armor gets refueled to 200%, with max of 200%
		ARM2,
		///Saves from damage caused by toxic floor
		SUIT,
		///Increases health by 10% up to 100%
		STIM,
};

#pragma region Weapons & Ammo
class WeaponCollectible : public Entity {
	CollectibleType type;
public:
	WeaponCollectible(svec2 pos, CollectibleType type);

	bool AllowCollection() const override;

	void Collect() override;
};

#define BfugCollectible(pos) WeaponCollectible(pos, CollectibleType::BFUG)
#define MinigunCollectible(pos) WeaponCollectible(pos, CollectibleType::MGUN)
#define ChainsawCollectible(pos) WeaponCollectible(pos, CollectibleType::CSAW)
#define PlasmaCollectible(pos) WeaponCollectible(pos, CollectibleType::PLAS)
#define LauncherCollectible(pos) WeaponCollectible(pos, CollectibleType::LAUN)
#define ShotgunCollectible(pos) WeaponCollectible(pos, CollectibleType::SHOT)
#define SShotgunCollectible(pos) WeaponCollectible(pos, CollectibleType::SGN2)
#define ShellCollectible(pos) WeaponCollectible(pos, CollectibleType::SHEL)
#define BoxOfRocketsCollectible(pos) WeaponCollectible(pos, CollectibleType::BROK)
#define BoxOfShellsCollectible(pos) WeaponCollectible(pos, CollectibleType::SBOX)
#define BoxOfBulletsCollectible(pos) WeaponCollectible(pos, CollectibleType::AMMO)
#define BulletsCollectible(pos) WeaponCollectible(pos, CollectibleType::CLIP)
#define CellCollectible(pos) WeaponCollectible(pos, CollectibleType::CELL)
#define BoxOfCellsCollectible(pos) WeaponCollectible(pos, CollectibleType::CELP)
#define RocketCollectible(pos) WeaponCollectible(pos, CollectibleType::Rock)

#pragma endregion

#pragma region Armors

class ArmorCollectibleC : public AnimatedEntity {
	uint16_t bonus;
	uint16_t max_val;
public:
	ArmorCollectibleC(svec2 pos, uint16_t bonus, uint16_t max_val, std::string base, std::string tex_sequence);

	bool AllowCollection() const override;

	void Collect() override;

};

#define ArmorBonusCollectible(pos) ArmorCollectibleC(pos, 1, 200, "BON2", "ABCDCB");
#define ArmorCollectible(pos) ArmorCollectibleC(pos, 100, 100, "ARM1", "AB");
#define MegaArmorCollectible(pos) ArmorCollectibleC(pos, 200, 200, "ARM2", "AB");

#pragma endregion

#pragma region Health
class BasicHealers : public AnimatedEntity{
	uint16_t bonus;
	uint16_t max_val;
	public:
	BasicHealers(svec2 pos, uint16_t bonus, uint16_t max_val, std::string base, std::string tex_sequence);

	bool AllowCollection() const override;

	void Collect() override;

};

#define MediKit(pos) BasicHealers(pos, 25, 100, "MEDI", "A");
#define StimPack(pos) BasicHealers(pos, 10, 100, "STIM", "A");
#define BottleOfWater(pos) BasicHealers(pos, 1, 200, "BON1", "ABCDCB");
#define Supercharge(pos) BasicHealers(pos, 100, 200, "SOUL", "ABCDCB");
#pragma endregion Health
#endif //DOOM_COLLECTIBLES_H