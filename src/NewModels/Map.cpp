//
// Created by Wiktor on 22.11.2025.
//
#include "../headers/Map.h"

NewModels::Map::Map() {
	texture_binder = new TexBinder();
}

void NewModels::Map::Render() {

	for (const auto& wall : walls) {
		wall->Render();
	}

	for (auto& sector : sectors) {
		sector.Render(last_player_pos);
	}

}

void NewModels::Map::Update(double deltaTime) {
	//TODO - Projectiles and monsters;
}

NewModels::Sector * NewModels::Map::getPlayerSector(svec2 pos, Sector *previousSector) {
	if (previousSector == nullptr) {
	getSector:
		svec3 pos3 = svec3(pos.x, pos.y,0);
		for (auto& sector : sectors) {
			if (sector.isInSector(static_cast<fvec3>(pos3)))
				return &sector;
		}
		return nullptr;
	}
	if (previousSector->isInSector(fvec3(pos.x, pos.y,0.)))
		return previousSector;
	for( auto& sec : previousSector->neighbors)
		if (sec->isInSector(fvec3(pos.x, pos.y,0.)))
			return sec;
	goto getSector;
}

void NewModels::Map::HandleMovement(fvec3 &move, fvec3 &player_pos, Sector *&currentSector) {
	while (!move.is_zero()) {
		//todo glicz przy wchodzeniu do ściany

		fvec3 p_pos = fvec3(player_pos.x, player_pos.y, player_pos.z);
		fvec3 player_pos_save = player_pos;

		fvec3 mv1 = move;
		p_pos = RayCaster::MovementRayCast(mv1, currentSector, p_pos, RayCaster::Walk);

		//test2
		player_pos = p_pos;
		move = mv1;

		svec3 p_pos_s = (svec3)player_pos;
		auto next_sector = getPlayerSector({p_pos_s.x, p_pos_s.z}, currentSector);
		if (next_sector == nullptr)
			player_pos = player_pos_save;
		else
			currentSector = next_sector;
	}
}

void NewModels::Map::TryActivateRay(fvec3 &lookVector, Sector *currentSector, fvec3 &start_pos) {
	bool target_hit;
	svec3 start_pos_s = (svec3)start_pos;
	//todo anty
	// RayCaster::PerformRayCast( lookVector, currentSector, start_pos_s, RayCaster::Interaction, target_hit);
}

NewModels::Map::~Map() {
	for (const auto& wall : walls) {
		delete wall;
	}
}
