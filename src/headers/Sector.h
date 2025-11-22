//
// Created by Wiktor on 22.11.2025.
//

#ifndef DOOM_SECTOR_H
#define DOOM_SECTOR_H
#include <vector>

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

		void BindTextureCoords(svec2 coords, gl_texture* texture) {
			fvec2 diff = {(float)coords.x/(float)texture->w, (float)coords.y/(float)texture->h};
			if (id==45 && !have_print)
				printf(" Tex Coords : (%f %f)\n", diff.x, -diff.y);
			glTexCoord2f(diff.x, -diff.y);
		}

		void Render(fvec2 playerpos) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);  // Standard: cull backs for both
			// Ceilings
			glBindTexture(GL_TEXTURE_2D, ceil_texture.texture_id);
			glBegin(GL_TRIANGLES);
			for (auto& line: lines) {
				// REVERSE ORDER for ceil: v3, v2, v1 -> CCW from below
				auto p = nodes[line.v1];
				if (id==45 && !have_print)
					printf("\n Real Coords : (%hd %hd)", p.x, -p.y);
				BindTextureCoords(p, &ceil_texture); glVertex3f(p.x, ceil_height, -p.y);

				p = nodes[line.v2];
				if (id==45 && !have_print)
					printf(" Real Coords : (%hd %hd)", p.x, -p.y);
				BindTextureCoords(p, &ceil_texture); glVertex3f(p.x, ceil_height, -p.y);

				p = nodes[line.v3];
				if (id==45 && !have_print)
					printf(" Real Coords : (%hd %hd)", p.x, -p.y);
				BindTextureCoords(p, &ceil_texture); glVertex3f(p.x, ceil_height, -p.y);
			}
			glEnd();
			// Floors (normal order)
			glBindTexture(GL_TEXTURE_2D, floor_texture.texture_id);
			glBegin(GL_TRIANGLES);  // Move outside loop for efficiency
			for (auto& line: lines) {
				auto p = nodes[line.v3];
				if (id==45 && !have_print)
					printf("\n Real Coords : (%hd %hd)", p.x, -p.y);
				BindTextureCoords(p, &floor_texture); glVertex3f(p.x, floor_height, -p.y);

				p = nodes[line.v2];
				if (id==45 && !have_print)
					printf(" Real Coords : (%hd %hd)", p.x, -p.y);
				BindTextureCoords(p, &floor_texture); glVertex3f(p.x, floor_height, -p.y);

				p = nodes[line.v1];
				if (id==45 && !have_print)
					printf(" Real Coords : (%hd %hd)", p.x, -p.y);
				BindTextureCoords(p, &floor_texture); glVertex3f(p.x, floor_height, -p.y);
			}
			have_print = true;
			glEnd();

			for (auto& entity: entities)
				entity->Render(playerpos);
			glDisable(GL_CULL_FACE);
		}

		void Update(double deltaTime) {
			for (auto& entity : entities) {
				if (entity->isToRemove()) {
					delete entity;
					entities.erase(entity);
				}
				entity->Update(deltaTime);
			}
		}

		bool isInSector(fvec3 pos) {
			if (pos.x < bounding_box[0].x || pos.x > bounding_box[1].x || pos.y < bounding_box[0].y || pos.y>bounding_box[1].y) return false;

			for (auto& triangle : lines) {
				if (isInTriangle(svec3(nodes[triangle.v1].x,nodes[triangle.v1].y, 0),
					svec3(nodes[triangle.v2].x,nodes[triangle.v2].y, 0),
					svec3(nodes[triangle.v3].x,nodes[triangle.v3].y, 0), pos)) return true;
			}
			return false;
		}

		void bindTextures(gl_texture floor_texture, gl_texture ceil_texture) {
			this->floor_texture = floor_texture;
			this->ceil_texture = ceil_texture;
		}

		std::vector<Wall*>& GetWalls() {
			return walls;
		}

		void bindWall(Wall* wall) {
			walls.push_back(wall);
		}

		~Sector() {
		}
	};
}


#endif //DOOM_SECTOR_H