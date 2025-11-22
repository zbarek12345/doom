//
// Created by Wiktor on 22.11.2025.
//

#ifndef DOOM_MAP_H
#define DOOM_MAP_H
#include "Projectile.h"
#include "Sector.h"
#include "TexBinder.h"
#include "Wall.h"

namespace NewModels {
	 class Map{
	 		static std::vector<Projectile*> projectiles_to_delete;
		public:
			static std::vector<Sector> sectors;
    		static std::vector<Wall*> walls;
    		//std::vector<Entity*> entities;
    		static std::set<ActionPerformer*> actions;
    		static std::set<Projectile*> projectiles;
			static svec3 player_start;
			static uint16_t player_start_angle;
			static TexBinder* texture_binder;

			Map();

			void Render();

		    static void Update(double deltaTime);

			static Sector* getPlayerSector(svec2 pos, Sector* previousSector);

			static void HandleProjectile(Projectile* projectile,float bullet_distance);

			void HandleMovement(fvec3& move, fvec3& player_pos, Sector*& currentSector);

			void TryActivateRay(fvec3& lookVector, Sector* currentSector, fvec3& start_pos);

			~Map();
	 };
}
#endif //DOOM_MAP_H