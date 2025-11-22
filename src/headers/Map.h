//
// Created by Wiktor on 22.11.2025.
//

#ifndef DOOM_MAP_H
#define DOOM_MAP_H
#include "Sector.h"
#include "Wall.h"

namespace NewModels {
	 class Map{
		public:
			static std::vector<Sector> sectors;
    		static std::vector<Wall*> walls;
    		//std::vector<Entity*> entities;
    		static std::set<ActionPerformer*> actions;
    		static std::set<Projectile*> projectiles;
			svec3 player_start = {0,0,0};
			uint16_t player_start_angle;
			TexBinder* texture_binder;

			Map();

			void Render();

			void Update(double deltaTime);

			Sector* getPlayerSector(svec2 pos, Sector* previousSector);

			static void HandleProjectile(Projectile* projectile);

			void HandleMovement(fvec3& move, fvec3& player_pos, Sector*& currentSector);

			void TryActivateRay(fvec3& lookVector, Sector* currentSector, fvec3& start_pos);

			~Map();
	 };
}
#endif //DOOM_MAP_H