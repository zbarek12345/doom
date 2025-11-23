//
// Created by Wiktor on 23.11.2025.
//

#include "headers/SpecialBinder.h"

#include "headers/ActionPerformer.h"
#include "headers/Map.h"
#include "headers/Wall.h"

std::map<uint16_t, std::set<NewModels::Sector*>> SpecialBinder::tags = {};

void SpecialBinder::CreateSpecial(const NewModels::Wall *activator, ActivationType t) {

	auto special_type = activator->getSpecialType();
	if (special_type == 0)
		return;

	if (special_type == 1) {
		auto action = new NewModels::DoorAction(NewModels::DoorAction::OpenClose, NewModels::DoorAction::Up, 100);
		auto other_sector = activator->GetLeftSector();
		action->BindTargets(other_sector);
		NewModels::Map::TryAddAction(action, other_sector, special_type);
	}
}

void SpecialBinder::BindSpecial(uint16_t tag, NewModels::Sector *sector) {
	tags[tag].emplace(sector);
}

void SpecialBinder::Clear() {
	tags.clear();
}
