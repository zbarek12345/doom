//
// Created by Wiktor on 05.10.2025.
//

#ifndef NEW_MODELS_H
#define NEW_MODELS_H
#include<vector>
#include<stdint.h>
#include <algorithm>
#include <gl/gl.h>

namespace NewModels{
	struct vec3 {
		int16_t x, y, z;
		bool operator==(const vec3& other) const {
			return  x == other.x &&
					y == other.y &&
					z == other.z;
		}
	};

	struct vec2 {
		int16_t x, y;


		bool operator<(vec2 other) const {
			if (x == other.x)
				return y < other.y;
			return x < other.x;
		}

		bool operator==(vec2 other) const {
			return x == other.x && y == other.y;
		}

	};

	struct wall{
          vec3* left_top, *right_top, * right_bottom, * left_bottom;
		  uint8_t type;
          //texture* texture;
          //int16_t tex_offset_x, tex_offset_y;
    };

	class Sector{
	public:
        int id{};
		int16_t ceil_height{}, floor_height{};
    	std::vector<vec3> ceiling;
        std::vector<vec3> floor;
		std::vector<std::vector<uint16_t>> lines;

        std::vector<wall> walls;

		Sector() {
			lines = std::vector<std::vector<uint16_t>>();
			ceiling = std::vector<vec3>();
			floor = std::vector<vec3>();
			walls = std::vector<wall>();
		}

		enum type {
			Ceiling,
			Floor,
		};

		inline vec3*	getPointer(vec2 pos, type t) {
			vec3* res = nullptr;
			vec3 temp = vec3{0, 0, 0};
			switch (t) {
				case Ceiling:
					temp = {pos.x , pos.y, ceil_height};
					res = &(*std::find(ceiling.begin(), ceiling.end(), temp));
					break;
				case Floor:
					temp = {pos.x , pos.y, floor_height};
					res = &(*std::find(floor.begin(), floor.end(), temp));
			}
			return res;
		}

		inline void Render() {
			///draw ceils
			glColor3ub(0,255,0);
			for (auto& line: lines) {
				glBegin(GL_POLYGON);
				for (auto& point: line) {
					auto p = ceiling[point];
					glVertex3s(p.x, p.z, -p.y);
				}
				glEnd();
			}

			///draw floors
			glColor3ub(255,0,0);
			for (auto& line: lines) {
				glBegin(GL_POLYGON);
				for (auto& point: line) {
					auto p = ceiling[point];
					glVertex3s(p.x, p.z, -p.y);
				}
				glEnd();
			}


			///Draw walls
			for (auto& wall: walls) {
				if (wall.type == 0)
					glColor3ub(0, 188, 209);
				else if (wall.type==1)
					glColor3ub(186, 58, 11);
				else
					glColor3ub(235, 73, 183);

				glBegin(GL_QUADS);
					glVertex3s(wall.left_top->x, wall.left_top->z, -wall.left_top->y);
					glVertex3s(wall.right_top->x, wall.right_top->z, -wall.right_top->y);
					glVertex3s(wall.right_bottom->x, wall.right_bottom->z, -wall.right_bottom->y);
					glVertex3s(wall.left_bottom->x, wall.left_bottom->z, -wall.left_bottom->y);
				glEnd();
			}


			//draw lines
			glLineWidth(4);
			glColor3ub(255, 255, 255);

			//ceiling
			for (auto& line: lines) {
				glBegin(GL_LINE_STRIP);
				for (auto& point: line) {
					auto p = ceiling[point];
					glVertex3s(p.x, p.z, -p.y);
				}
				glEnd();
			}
			//floor
			for (auto& line: lines) {
				glBegin(GL_LINE_STRIP);
				for (auto& point: line) {
					auto p = ceiling[point];
					glVertex3s(p.x, p.z, -p.y);
				}
				glEnd();
			}
			//Walls
			for (auto& wall: walls) {
				glBegin(GL_LINE_STRIP);
					glVertex3s(wall.left_top->x, wall.left_top->z, wall.left_top->y);
					glVertex3s(wall.right_top->x, wall.right_top->z, wall.right_top->y);
					glVertex3s(wall.right_bottom->x, wall.right_bottom->z, wall.right_bottom->y);
					glVertex3s(wall.left_bottom->x, wall.left_bottom->z, wall.left_bottom->y);
					glVertex3s(wall.left_top->x, wall.left_top->z, wall.left_top->y);
				glEnd();
			}
		}
    };

    class Map{
      public:
        std::vector<Sector> sectors;
		vec3 player_start;

      Map() {
	    sectors = std::vector<Sector>();
      	player_start = {0,0,0};
      }

      void render() {
	      for (auto& sector : sectors) {
		      sector.Render();
	      }
      }
    };
};
#endif //NEW_MODELS_H
