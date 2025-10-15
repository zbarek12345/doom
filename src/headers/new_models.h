//
// Created by Wiktor on 05.10.2025.
//

#ifndef NEW_MODELS_H
#define NEW_MODELS_H
#include<vector>
#include<stdint.h>
#include <algorithm>
#include <CDTUtils.h>
#include <gl/gl.h>

#include "map.h"
#include "original_classes.h"
#include "TexBinder.h"

namespace NewModels{

#define TEX_MULTIPLIER 8

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
		  int16_t tex_offset_x, tex_offset_y;
		  uint8_t type;
          GLuint texture;
          //int16_t tex_offset_x, tex_offset_y;
    };

	struct triangle {
		uint16_t v1, v2, v3;
	};

	class Sector{
	public:
        uint16_t id{};
		int16_t ceil_height{}, floor_height{};
		vec2 bounding_box[2];
    	std::vector<vec3> ceiling;
        std::vector<vec3> floor;
		std::vector<triangle> lines;

		CDT::EdgeUSet outer_edges;
		std::vector<uint16_t> edges_map;
		original_map* map;

        std::vector<wall> walls;

		enum type {
			Ceiling,
			Floor,
		};

		inline vec3* getPointer(vec2 pos, type t) {
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

		bool calculateDistance(vec2 pos, int id){
			auto it = outer_edges.begin();

			auto line = *it;

			auto min_edge = outer_edges.begin();
			double min_distance = std::numeric_limits<double>::max();
			for (auto it = outer_edges.begin(); it != outer_edges.end(); ++it) {
				auto &edge = *it;

				vec2 start, end;

				start = vec2{ceiling[(uint16_t)edge.v1()].x, ceiling[(uint16_t)edge.v1()].y};
				end = vec2{ceiling[(uint16_t)edge.v2()].x, ceiling[(uint16_t)edge.v2()].y};

				// Calculate the distance from pos to the line segment
				vec2 line_vec{(int16_t) (end.x - start.x), (int16_t) (end.y - start.y)};
				vec2 point_vec{(int16_t) (pos.x - start.x), (int16_t) (pos.y - start.y)};

				double dot = line_vec.x * point_vec.x + line_vec.y * point_vec.y;
				double length_sq = line_vec.x * line_vec.x + line_vec.y * line_vec.y;
				double t = (length_sq == 0) ? 0 : dot / length_sq;

				t = std::max(0.0, std::min(1.0, t));

				vec2 projection{
					(int16_t) (start.x + t * line_vec.x),
					(int16_t) (start.y + t * line_vec.y)
				};

				double distance = std::sqrt(
					(projection.x - pos.x) * (projection.x - pos.x) +
					(projection.y - pos.y) * (projection.y - pos.y)
				);

				if (min_distance>distance) {
					min_distance = distance;
					min_edge = it;
				}
			}

			auto p1 = it->v1(), p2 = it->v2();



		}

		bool isInTriangle(vec2 pos, triangle& t) {
			bool isInside = false;

			auto v1 = vec2{ceiling[t.v1].x, ceiling[t.v1].y};
			auto v2 = vec2{ceiling[t.v2].x, ceiling[t.v2].y};
			auto v3 = vec2{ceiling[t.v3].x, ceiling[t.v3].y};

			auto v1v2 = vec2{(int16_t)(v2.x-v1.x), (int16_t)(v2.y-v1.y)};
			auto v2v3 = vec2{(int16_t)(v3.x-v2.x), (int16_t)(v3.y-v2.y)};
			auto v3v1 = vec2{(int16_t)(v1.x-v3.x), (int16_t)(v1.y-v3.y)};

			auto tv1 = vec2{(int16_t)(pos.x-v1.x), (int16_t)(pos.y-v1.y)};
			auto tv2 = vec2{(int16_t)(pos.x-v2.x), (int16_t)(pos.y-v2.y)};
			auto tv3 = vec2{(int16_t)(pos.x-v3.x), (int16_t)(pos.y-v3.y)};

			auto cp1 = v1v2.x*tv1.y - v1v2.y*tv1.x;
			auto cp2 = v2v3.x*tv2.y - v2v3.y*tv2.x;
			auto cp3 = v3v1.x*tv3.y - v3v1.y*tv3.x;

			if (cp1 >= 0 && cp2 >= 0 && cp3 >= 0 || cp1 <= 0 && cp2 <= 0 && cp3 <= 0)
				isInside = true;
			return isInside;
		}

		bool isInside(vec2 pos) {
			if (pos.x>bounding_box[1].x || pos.x<bounding_box[0].x || pos.y>bounding_box[1].y || pos.y<bounding_box[0].y)
				return false;

			// for (auto& line: lines) {
			// 	if (isInTriangle(pos, line))
			// 		return true;
			// }
			return true;
		}

		inline void Render() {
			///draw ceils
			glColor3ub(0,255,0);

			for (auto& line: lines) {
				glBegin(GL_TRIANGLES);
				auto p = ceiling[line.v1];
				glVertex3s(p.x, p.z, -p.y);

				p = ceiling[line.v2];
				glVertex3s(p.x, p.z, -p.y);

				p = ceiling[line.v3];
				glVertex3s(p.x, p.z, -p.y);
				glEnd();

			}

			///draw floors
			glColor3ub(255,0,0);

			for (auto& line: lines) {
				glBegin(GL_TRIANGLES);
				auto p = floor[line.v1];
				glVertex3s(p.x, p.z, -p.y);

				p = floor[line.v2];
				glVertex3s(p.x, p.z, -p.y);

				p = floor[line.v3];
				glVertex3s(p.x, p.z, -p.y);
				glEnd();
			}
			glColor3ub(255, 255, 255);
			glEnable(GL_TEXTURE_2D);
			///Draw walls
			for (auto& wall: walls) {

				if (wall.texture != 0)
					glBindTexture(GL_TEXTURE_2D, wall.texture);
				GLint tex_w, tex_h;
				glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_w);
				glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_h);

				float wall_len = std::sqrt(std::pow(wall.right_bottom->x - wall.left_bottom->x, 2) + std::pow(wall.right_bottom->y - wall.left_bottom->y, 2))*TEX_MULTIPLIER;
				float wall_height = (wall.left_top->z - wall.left_bottom->z)*TEX_MULTIPLIER;

				float u_left = static_cast<float>(wall.tex_offset_x) / tex_w;
				float u_right = u_left + wall_len / tex_w;
				float v_bottom = static_cast<float>(wall.tex_offset_y) / tex_h;
				float v_top = v_bottom + wall_height / tex_h;

				glBegin(GL_QUADS);
					glTexCoord2f(u_left, v_bottom);    glVertex3s(wall.left_top->x, wall.left_top->z, -wall.left_top->y);
					glTexCoord2f(u_right, v_bottom);   glVertex3s(wall.right_top->x, wall.right_top->z, -wall.right_top->y);
					glTexCoord2f(u_right, v_top); glVertex3s(wall.right_bottom->x, wall.right_bottom->z, -wall.right_bottom->y);
					glTexCoord2f(u_left, v_top);  glVertex3s(wall.left_bottom->x, wall.left_bottom->z, -wall.left_bottom->y);
				glEnd();
			}
			glDisable(GL_TEXTURE_2D);


			//draw lines
			glLineWidth(4);
			glColor3ub(255, 255, 255);

			glBegin(GL_LINE_STRIP);
			//ceiling
			for (auto& line: lines) {

				auto p = ceiling[line.v1];
				glVertex3s(p.x, p.z, -p.y);

				p = ceiling[line.v2];
				glVertex3s(p.x, p.z, -p.y);

				p = ceiling[line.v3];
				glVertex3s(p.x, p.z, -p.y);

				p = ceiling[line.v1];
				glVertex3s(p.x, p.z, -p.y);
			}
			glEnd();
			//floor
			glBegin(GL_LINE_STRIP);
			for (auto& line: lines) {
				auto p = floor[line.v1];
				glVertex3s(p.x, p.z, -p.y);

				p = floor[line.v2];
				glVertex3s(p.x, p.z, -p.y);

				p = floor[line.v3];
				glVertex3s(p.x, p.z, -p.y);

				p = floor[line.v1];
				glVertex3s(p.x, p.z, -p.y);
			}
			glEnd();
			//Walls
			// for (auto& wall: walls) {
			// 	glBegin(GL_LINE_STRIP);
			// 		glVertex3s(wall.left_top->x, wall.left_top->z, wall.left_top->y);
			// 		glVertex3s(wall.right_top->x, wall.right_top->z, wall.right_top->y);
			// 		glVertex3s(wall.right_bottom->x, wall.right_bottom->z, wall.right_bottom->y);
			// 		glVertex3s(wall.left_bottom->x, wall.left_bottom->z, wall.left_bottom->y);
			// 		glVertex3s(wall.left_top->x, wall.left_top->z, wall.left_top->y);
			// 	glEnd();
			// }
		}
    };

    class Map{
      public:
        std::vector<Sector> sectors;
		vec3 player_start;
    	uint16_t player_start_angle;
    	TexBinder* texture_binder;

      Map() {
	      texture_binder = new TexBinder();
      }

      void render() {
	      for (auto& sector : sectors) {
		      sector.Render();
	      }
      }
    };
};
#endif //NEW_MODELS_H
