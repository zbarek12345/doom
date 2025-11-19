//
// Created by Wiktor on 18.11.2025.
//

#include "headers/Collectibles.h"

#include "headers/Player.h"

WeaponCollectible::WeaponCollectible(svec2 pos, CollectibleType type)
	: Entity(pos, 20,
			 [type]() -> std::string {
				 switch (type) {
					 case CollectibleType::BFUG:
				 		return "BFUG";
					 case CollectibleType::MGUN:
				 		return "MGUN";
					 case CollectibleType::CSAW:
				 		return "CSAW";
					 case CollectibleType::PLAS:
				 		return "PLAS";
					 case CollectibleType::LAUN:
				 		return "LAUN";
					 case CollectibleType::SHOT:
				 		return "SHOT";
					 case CollectibleType::SGN2:
				 		return "SGN2";
					 case CollectibleType::SHEL:
				 		return "SHEL";
					 case CollectibleType::BROK:
				 		return "BROK";
				 	 case CollectibleType::SBOX:
				 		return "SBOX";
					 case CollectibleType::AMMO:
				 		return "AMMO";
				 	 case CollectibleType::CLIP:
				 		return "CLIP";
				 	 case CollectibleType::CELL:
				 		return "CELL";
				 	 case CollectibleType::CELP:
				 		return "CELP";
				 	 case CollectibleType::ROCK:
				 		return "ROCK";
					 default:
				 		return "";  // Or handle error, e.g., throw std::invalid_argument("Invalid type");
				 }
			 }(),
			 "A", false, EntityPosType::Floor),
	  type(type) {
}

bool WeaponCollectible::AllowCollection() const {
	auto wepId = 0;
	auto ammoid = 0;
	uint16_t ammo_amount = 0;
	switch (type) {
		case CollectibleType::BFUG:
			ammoid = 3;
			wepId = static_cast<int>(Weapon::BFG);
			break;
		case CollectibleType::MGUN:
			ammoid = 0;
			wepId = static_cast<int>(Weapon::Minigun);
			break;
		case CollectibleType::CSAW:
			wepId = static_cast<int>(Weapon::Chainsaw);
			break;
		case CollectibleType::PLAS:
			ammoid = 3;
			wepId = static_cast<int>(Weapon::Plasma);
			break;
		case CollectibleType::LAUN:
			ammoid = 2;
			wepId = static_cast<int>(Weapon::Launcher);
			break;
		case CollectibleType::SHOT:
			ammoid = 1;
			wepId = static_cast<int>(Weapon::Shotgun);
			break;
		case CollectibleType::SGN2:
			ammoid = 1;
			wepId = static_cast<int>(Weapon::SShotgun);
			break;
		case CollectibleType::SHEL:
			ammoid = 1;
			break;
		case CollectibleType::BROK:
			ammoid = 3;
			break;
		case CollectibleType::SBOX:
			ammoid = 1;
			break;
		case CollectibleType::AMMO:
			ammoid = 0;
			break;
		case CollectibleType::CLIP:
			ammoid = 0;
			break;
		case CollectibleType::CELL:
			ammoid = 3;
			break;
		case CollectibleType::CELP:
			ammoid = 3;
			break;
		case CollectibleType::ROCK:
			ammoid = 2;
			break;
		default: ;
	}

	return !Player::has_weapon[wepId] || Player::ammo[ammoid] < ammo_amount;
}

void WeaponCollectible::Collect() const {

	auto wepId = 0;
	auto ammoid = 0;
	uint16_t ammo_amount = 0;
	bool collects = false;
	switch (type) {
		case CollectibleType::BFUG:
			ammoid = 3;
			ammo_amount = 40;
			collects = true;
			wepId = static_cast<int>(Weapon::BFG);
			break;
		case CollectibleType::MGUN:
			ammoid = 0;
			ammo_amount = 20;
			collects = true;
			wepId = static_cast<int>(Weapon::Minigun);
			break;
		case CollectibleType::CSAW:
			collects = true;
			wepId = static_cast<int>(Weapon::Chainsaw);
			break;
		case CollectibleType::PLAS:
			ammoid = 3;
			ammo_amount = 20;
			collects = true;
			wepId = static_cast<int>(Weapon::Plasma);
			break;
		case CollectibleType::LAUN:
			ammoid = 2;
			ammo_amount = 2;
			collects = true;
			wepId = static_cast<int>(Weapon::Launcher);
			break;
		case CollectibleType::SHOT:
			ammoid = 1;
			ammo_amount = 8;
			collects = true;
			wepId = static_cast<int>(Weapon::Shotgun);
			break;
		case CollectibleType::SGN2:
			ammoid = 1;
			ammo_amount = 8;
			collects = true;
			wepId = static_cast<int>(Weapon::SShotgun);
			break;
		case CollectibleType::SHEL:
			ammoid = 1;
			ammo_amount = 4;
			break;
		case CollectibleType::BROK:
			ammoid = 3;
			ammo_amount = 5;
			break;
		case CollectibleType::SBOX:
			ammoid = 1;
			ammo_amount = 20;
			break;
		case CollectibleType::AMMO:
			ammoid = 0;
			ammo_amount = 50;
			break;
		case CollectibleType::CLIP:
			ammoid = 0;
			ammo_amount = 10;
			break;
		case CollectibleType::CELL:
			ammoid = 3;
			ammo_amount = 20;
			break;
		case CollectibleType::CELP:
			ammoid = 3;
			ammo_amount = 100;
			break;
		case CollectibleType::ROCK:
			ammoid = 2;
			ammo_amount = 1;
			break;
		default: ;
	}

	if (collects) {
		Player::has_weapon[wepId] = true;
	}
	else {
		Player::ammo[ammoid]+=std::min(static_cast<uint16_t>(Player::ammo[ammoid]+ammo_amount), Player::max_ammo[ammoid]);
	}
}

ArmorCollectibleC::ArmorCollectibleC(svec2 pos, uint16_t bonus, uint16_t max_val, std::string base,
	std::string tex_sequence):AnimatedEntity(pos, 20, base, tex_sequence, false, EntityPosType::Floor), bonus(bonus), max_val(max_val) {
}

bool ArmorCollectibleC::AllowCollection() const {
	return Player::armor < max_val;
}

void ArmorCollectibleC::Collect() const {
	Player::armor=std::min(static_cast<uint16_t>(Player::armor+bonus), max_val);
}

BasicHealers::BasicHealers(svec2 pos, uint16_t bonus, uint16_t max_val, std::string base, std::string tex_sequence)
	: AnimatedEntity(pos, 20, base, tex_sequence, false, EntityPosType::Floor), bonus(bonus), max_val(max_val) {}

bool BasicHealers::AllowCollection() const {
	return Player::health < max_val;
}

void BasicHealers::Collect() const {
	Player::health=std::min(static_cast<uint16_t>(Player::health+bonus), max_val);
}

