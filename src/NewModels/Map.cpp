//
// Created by Wiktor on 22.11.2025.
//
#include "../headers/Map.h"

#include "src/headers/Player.h"
#include "src/headers/RayCaster.h"

std::vector<Projectile*> NewModels::Map::projectiles_to_delete = {};
std::vector<NewModels::Sector> NewModels::Map::sectors = {};
std::vector<NewModels::Wall*> NewModels::Map::walls = {};
//std::vector<Entity*> entities;
std::set<NewModels::ActionPerformer*> NewModels::Map::actions = {};
std::set<Projectile*> NewModels::Map::projectiles = {};
svec3 NewModels::Map::player_start = {0,0,0};
uint16_t NewModels::Map::player_start_angle = 0;
TexBinder* NewModels::Map::texture_binder = nullptr;

NewModels::Map::Map() {
	texture_binder = new TexBinder();
}

void NewModels::Map::Render() {
	glEnable(GL_TEXTURE_2D);
	for (const auto& wall : walls) {
		wall->Render();
	}

	for (auto& sector : sectors) {
		sector.Render();
	}

	for (auto& projectile : projectiles) {
		projectile->Render();
	}
	glDisable(GL_TEXTURE_2D);
}

void NewModels::Map::Update(double deltaTime) {
	for (auto& projectile : projectiles) {
		projectile->Update(deltaTime);
	}

	for (auto& projectile : projectiles_to_delete) {
		projectiles.erase(projectile);
		delete projectile;
	}
	projectiles_to_delete.clear();
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

void NewModels::Map::HandleProjectile(Projectile *projectile, float bullet_distance) {
	fvec3 pos, dir;
	projectile->GetDetails(dir, pos);
	auto psector = projectile->GetSector();
	auto target_hit = false;
	dir*=bullet_distance;
	auto ret = RayCaster::PerformRayCast(dir, psector, pos, RayCaster::Shot, target_hit, projectile->GetCaster());

	if (target_hit) {
		projectiles_to_delete.push_back(projectile);
	}else{
		projectile->SetSector(psector);
		projectile->SetPosition(ret);
	}
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
