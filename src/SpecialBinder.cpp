//
// Created by Wiktor on 23.11.2025.
//

#include "headers/SpecialBinder.h"

#include "headers/ActionPerformer.h"
#include "headers/Map.h"
#include "headers/Player.h"
#include "headers/Wall.h"

std::map<uint16_t, std::set<NewModels::Sector*>> SpecialBinder::tags = {};

void SpecialBinder::CreateSpecial(NewModels::Wall *activator, ActivationType t) {

	auto special_type = activator->getSpecialType();
	if (special_type == 0)
		return;

	if (special_type == 1 && t == ActivationType::Trigger) {
		auto action = new NewModels::DoorAction(NewModels::DoorAction::OpenClose, NewModels::DoorAction::Up, 1);
		auto other_sector = activator->GetLeftSector();
		action->BindTargets(other_sector);
		NewModels::Map::TryAddAction(action, other_sector, special_type);
	}

	#pragma region Color Keys Open/Close

		if (special_type == 26 && t == ActivationType::Trigger) {
			if (!Player::keys[0])
				return;
			auto action = new NewModels::DoorAction(NewModels::DoorAction::OpenClose, NewModels::DoorAction::Up, 1);
			auto other_sector = activator->GetLeftSector();
			action->BindTargets(other_sector);
			NewModels::Map::TryAddAction(action, other_sector, special_type);
		}

		if (special_type == 27 && t == ActivationType::Trigger) {
			if (!Player::keys[2])
				return;
			auto action = new NewModels::DoorAction(NewModels::DoorAction::OpenClose, NewModels::DoorAction::Up, 1);
			auto other_sector = activator->GetLeftSector();
			action->BindTargets(other_sector);
			NewModels::Map::TryAddAction(action, other_sector, special_type);
		}

		if (special_type == 28 && t == ActivationType::Trigger) {
			if (!Player::keys[1])
				return;
			auto action = new NewModels::DoorAction(NewModels::DoorAction::OpenClose, NewModels::DoorAction::Up, 1);
			auto other_sector = activator->GetLeftSector();
			action->BindTargets(other_sector);
			NewModels::Map::TryAddAction(action, other_sector, special_type);
		}

	#pragma endregion
	if (special_type == 31) {
		auto action = new NewModels::DoorAction(NewModels::DoorAction::Open, NewModels::DoorAction::Up, 1);
		auto other_sector = activator->GetLeftSector();
		action->BindTargets(other_sector);
		NewModels::Map::TryAddAction(action, other_sector, special_type);
		activator->DeleteSpecialType();
	}

	if (special_type == 46 && t == ActivationType::Shot) {
		auto action = new NewModels::DoorAction(NewModels::DoorAction::Open, NewModels::DoorAction::Up, 255);
		auto other_sector = activator->GetLeftSector();
		action->BindTargets(other_sector);
		NewModels::Map::TryAddAction(action, other_sector, special_type);
		activator->DeleteSpecialType();
	}

	if (special_type == 88 && t == ActivationType::Walkover) {
		for (auto& sector : tags[activator->GetSectorTag()]) {
			auto action = new NewModels::LiftAction(2);
			action->BindTargets(sector);
			NewModels::Map::TryAddAction(action, sector, special_type);
		}
	}


	//
	if (special_type == 103) {
		for (auto& sector : tags[activator->GetSectorTag()]) {
			auto action = new NewModels::DoorAction(NewModels::DoorAction::Open, NewModels::DoorAction::Up, 1);
			action->BindTargets(sector);
			NewModels::Map::TryAddAction(action, sector, special_type);
		}
		activator->DeleteSpecialType();
	}
}

void SpecialBinder::BindSpecial(uint16_t tag, NewModels::Sector *sector) {
	tags[tag].emplace(sector);
}

void SpecialBinder::Clear() {
	tags.clear();
}
