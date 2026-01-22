//
// Created by Wiktor on 22.11.2025.
//

#ifndef DOOM_SECTOR_H
#define DOOM_SECTOR_H
#include <vector>
#include "Entity.h"
#include "new_models.h"
#include "texture.h"
#include "vec2.h"

namespace NewModels {
	class Sector{
	public:
		gl_texture floor_texture;
		gl_texture ceil_texture;

		uint16_t id{};
		int16_t ceil_height{}, floor_height{};
		svec2 bounding_box[2];
		std::vector<svec2> nodes;
		std::vector<triangle> lines;

		CDT::EdgeUSet outer_edges;
		std::vector<uint16_t> edges_map;
		std::set<Sector*> neighbors;
		std::vector<Wall*> walls={};
		std::set<Entity*> entities;
		bool have_print= false;

		enum type {
			Ceiling,
			Floor,
		};

		void BindTextureCoords(svec2 coords, gl_texture* texture) const;

		void Render();

		void Update(double deltaTime);

		bool isInSector(fvec3 pos);

		void bindTextures(gl_texture floor_texture, gl_texture ceil_texture);

		std::vector<Wall*>& GetWalls();

		void bindWall(Wall* wall);

		~Sector() {
		}
	};
}


#endif //DOOM_SECTOR_H