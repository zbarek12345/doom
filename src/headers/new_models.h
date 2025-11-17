//
// Created by Wiktor on 05.10.2025.
//

#ifndef NEW_MODELS_H
#define NEW_MODELS_H
#include<vector>
#include<stdint.h>
#include <algorithm>
#include <CDTUtils.h>
#include <climits>
#include <cstring>
#include <set>
#include <GL/gl.h>

#include "Entity.h"
#include "TexBinder.h"
#include "vec2.h"
#include "vec3.h"

namespace NewModels{
	class Wall;
	struct triangle;
	class Sector;
#define TEX_MULTIPLIER 8

	struct triangle {
		uint16_t v1, v2, v3;
	};

	inline bool isInTriangle(svec3 vert1, svec3 vert2, svec3 vert3, fvec3 point) {
		// Convert vertices to float for precision
		fvec3 v1 = fvec3((float)vert1.x, (float)vert1.y, (float)vert1.z);
		fvec3 v2 = fvec3((float)vert2.x, (float)vert2.y, (float)vert2.z);
		fvec3 v3 = fvec3((float)vert3.x, (float)vert3.y, (float)vert3.z);

		// Vectors from v1 to others
		fvec3 u = v2 - v1;
		fvec3 v = v3 - v1;
		fvec3 w = point - v1;

		// Precompute dot products
		float dot00 = fvec3::dot(u, u);
		float dot01 = fvec3::dot(u, v);
		float dot02 = fvec3::dot(u, w);
		float dot11 = fvec3::dot(v, v);
		float dot12 = fvec3::dot(v, w);

		// Compute denominator
		float denom = dot00 * dot11 - dot01 * dot01;
		if (fabsf(denom) < 1e-6f) return false; // Degenerate triangle

		// Barycentric coordinates
		float alpha = (dot11 * dot02 - dot01 * dot12) / denom;
		float beta  = (dot00 * dot12 - dot01 * dot02) / denom;
		float gamma = 1.0f - alpha - beta;

		// Check if point is inside (including edges)
		const float eps = 1e-6f;
		return (alpha >= -eps) && (beta >= -eps) && (gamma >= -eps);
	}

	enum special_activator_type : char {
		WalkOver,
		Interaction,
		Shot,
	};

	//todo zbidowana do linedef, wywolywanie akcji, aktywacja: walkover, click(ray), hit(tylko na ściane)
	class ActionPerformer {
		bool finished;
		uint8_t speed;
		std::vector<Sector*> sectors;

		bool IsFinished () {

		}

		void Update() {

		}
	};

	class Special {

	};

	class Sector{
		const float tex_size = 64.f;
	public:
		GLuint floor_texture;
		GLuint ceil_texture;

		uint16_t id{};
		int16_t ceil_height{}, floor_height{};
		svec2 bounding_box[2];
		std::vector<svec2> nodes;
		std::vector<triangle> lines;

		CDT::EdgeUSet outer_edges;
		std::vector<uint16_t> edges_map;
		std::set<Sector*> neighbors;
		std::vector<Wall*> walls={};
		bool have_print= false;

		enum type {
			Ceiling,
			Floor,
		};

		void BindTextureCoords(svec2 coords) {
			fvec2 diff = {coords.x/tex_size, coords.y/tex_size};
			if (id==45 && !have_print)
				printf(" Tex Coords : (%f %f)\n", diff.x, -diff.y);
			glTexCoord2f(diff.x, -diff.y);
		}

		void Render() {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);  // Standard: cull backs for both
			// Ceilings
			glBindTexture(GL_TEXTURE_2D, ceil_texture);
			glBegin(GL_TRIANGLES);
			for (auto& line: lines) {
				// REVERSE ORDER for ceil: v3, v2, v1 -> CCW from below
				auto p = nodes[line.v1];
				if (id==45 && !have_print)
					printf("\n Real Coords : (%hd %hd)", p.x, -p.y);
				BindTextureCoords(p); glVertex3f(p.x, ceil_height, -p.y);

				p = nodes[line.v2];
				if (id==45 && !have_print)
					printf(" Real Coords : (%hd %hd)", p.x, -p.y);
				BindTextureCoords(p); glVertex3f(p.x, ceil_height, -p.y);

				p = nodes[line.v3];
				if (id==45 && !have_print)
					printf(" Real Coords : (%hd %hd)", p.x, -p.y);
				BindTextureCoords(p); glVertex3f(p.x, ceil_height, -p.y);
			}
			glEnd();
			// Floors (normal order)
			glBindTexture(GL_TEXTURE_2D, floor_texture);
			glBegin(GL_TRIANGLES);  // Move outside loop for efficiency
			for (auto& line: lines) {
				auto p = nodes[line.v3];
				if (id==45 && !have_print)
					printf("\n Real Coords : (%hd %hd)", p.x, -p.y);
				BindTextureCoords(p); glVertex3f(p.x, floor_height, -p.y);

				p = nodes[line.v2];
				if (id==45 && !have_print)
					printf(" Real Coords : (%hd %hd)", p.x, -p.y);
				BindTextureCoords(p); glVertex3f(p.x, floor_height, -p.y);

				p = nodes[line.v1];
				if (id==45 && !have_print)
					printf(" Real Coords : (%hd %hd)", p.x, -p.y);
				BindTextureCoords(p); glVertex3f(p.x, floor_height, -p.y);
			}
			have_print = true;
			glEnd();
			glDisable(GL_CULL_FACE);
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

		void bindTextures(GLuint floor_texture, GLuint ceil_texture) {
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

	class Wall{

		Sector* this_sector;
		Sector* other_sector;

		struct wall_flags_t{
			bool Impassible : 1;
			bool BlockMonsters : 1;
			bool TwoSided : 1;
			bool UpperUnpegged : 1;
			bool LowerUnpegged : 1;
			bool Secret : 1;
			bool BlockSound : 1;
			bool NotOnMap: 1;
			bool AlreadyOnMap: 1;
			char rest : 7;
		} flags_;

		struct wall_tests_t{
		  bool upper_wall[2];
		  bool middle_wall[2];
		  bool lower_wall[2];
		  bool special_active : 1;
		}wall_tests;
		uint16_t special_type;

		GLuint lower_texture[2];
		GLuint middle_texture[2];
		GLuint upper_texture[2];

		int16_t offsets[4];
		uint16_t sector_tag;

		struct coordinates {
			svec2 v1, v2;
		}coordinates_;
	public:
		Wall(Sector* this_sector, Sector* other_sector) : this_sector(this_sector), other_sector(other_sector) {
			wall_tests.upper_wall[0] = false;
			wall_tests.upper_wall[1] = false;
			wall_tests.middle_wall[0] = false;
			wall_tests.middle_wall[1] = false;
			wall_tests.lower_wall[0] = false;
			wall_tests.lower_wall[1] = false;
			wall_tests.special_active = false;
			special_type = 0;
		}

		void setCoordinates(svec2 v1, svec2 v2) {
			coordinates_.v1 = v1;
			coordinates_.v2 = v2;
		}

		void assignLowerTexture(int id, GLuint texture) {
			lower_texture[id] = texture;
			wall_tests.lower_wall[id] = true;
		}

		void assignMiddleTexture(int id,GLuint texture) {
			middle_texture[id] = texture;
			wall_tests.middle_wall[id] = true;
		}

		void assignUpperTexture(int id, GLuint texture) {
			upper_texture[id] = texture;
			wall_tests.upper_wall[id] = true;
		}

		void assignSpecial(uint16_t type) {
			special_type = type;
			wall_tests.special_active = true;
		}

		void setOffsets(int id, int16_t xOffset, int16_t yOffset) {
			offsets[id] = xOffset;
			offsets[id + 1] = yOffset;
		}

		void* getSpecial(special_activator_type type);

		fvec3 getPlanePoint() {
			return fvec3(coordinates_.v1.x, coordinates_.v1.y,this_sector->floor_height).xzy();
		}

		fvec3 getPlaneNormal() {
			int16_t sectH = this_sector->ceil_height, sectL = this_sector->floor_height;
			svec3 p1 = {coordinates_.v1.x,coordinates_.v1.y,sectH}, p2={coordinates_.v1.x,coordinates_.v1.y,sectL}, p3={coordinates_.v2.x,coordinates_.v2.y,sectL};
			p1 = p1.xzy(); p2 = p2.xzy(); p3 = p3.xzy();
			fvec3 v1 = fvec3(p2-p1), v2 = fvec3(p3-p1);

			fvec3 cross = v1.cross(v2);
			cross.normalize();
			return cross;
		}

		bool Intersects(fvec3 point) {
			int16_t lowest, highest;

			if (other_sector != nullptr) {
				lowest = std::min(this_sector->floor_height, other_sector->floor_height);
				highest = std::max(this_sector->ceil_height, other_sector->ceil_height);
			}
			else {
				lowest = this_sector->floor_height;
				highest = this_sector->ceil_height;
			}

			bool t1 = isInTriangle(svec3(coordinates_.v1.x, coordinates_.v1.y, highest).xzy(),
						svec3(coordinates_.v2.x,coordinates_.v2.y, highest).xzy(),
						svec3(coordinates_.v2.x,coordinates_.v2.y, lowest).xzy(),
						point);

			bool t2 = isInTriangle(svec3(coordinates_.v1.x, coordinates_.v1.y, highest).xzy(),
									svec3(coordinates_.v2.x, coordinates_.v2.y, lowest).xzy(),
									svec3(coordinates_.v1.x, coordinates_.v1.y, lowest).xzy(), point);

			return t1 || t2;
		}

		void Render() {
            if (!this_sector) return;

            svec2 v1 = coordinates_.v1;
            svec2 v2 = coordinates_.v2;

            float wall_len = std::sqrt(std::pow(v2.x - v1.x, 2) + std::pow(v2.y - v1.y, 2)) * TEX_MULTIPLIER;

            if (this_sector == nullptr || other_sector == nullptr) {
                // One-sided: render middle as solid wall if texture assigned
                if (!wall_tests.middle_wall) return;

                glBindTexture(GL_TEXTURE_2D, middle_texture[0]);
                GLint tex_w, tex_h;
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_w);
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_h);

                float wall_height = (this_sector->ceil_height - this_sector->floor_height) * TEX_MULTIPLIER;

                // Basic texture coord calculation (add pegging logic here if needed)
                float u_left = static_cast<float>(offsets[0]) / tex_w;
                float u_right = u_left + wall_len / tex_w;
                float v_bottom = static_cast<float>(offsets[1]) / tex_h;  // v_bottom at top of wall
                float v_top = v_bottom + wall_height / tex_h;  // v_top at bottom of wall

                glBegin(GL_QUADS);
                glTexCoord2f(u_left, v_bottom);    glVertex3s(v1.x, this_sector->ceil_height, -v1.y);
                glTexCoord2f(u_right, v_bottom);   glVertex3s(v2.x, this_sector->ceil_height, -v2.y);
                glTexCoord2f(u_right, v_top);      glVertex3s(v2.x, this_sector->floor_height, -v2.y);
                glTexCoord2f(u_left, v_top);       glVertex3s(v1.x, this_sector->floor_height, -v1.y);
                glEnd();
            } else {
                // Two-sided wall

                // Render lower if this floor > other floor and texture assigned
                if (wall_tests.lower_wall[0] || wall_tests.lower_wall[1]) {
                	svec2 v1,v2;
                	Sector* this_sector, *other_sector;
                	GLuint lower_texture;
                	bool isTexture;
                	int16_t xoffset, yoffset;
                	if (this->this_sector->floor_height < this->other_sector->floor_height) {
                		this_sector = this->this_sector;
                		other_sector = this->other_sector;
                		v1 = this->coordinates_.v1;
                		v2 = this->coordinates_.v2;
                		lower_texture =this->lower_texture[0];
                		xoffset = this->offsets[0];
                		yoffset = this->offsets[1];
                		isTexture = wall_tests.lower_wall[0];
                	}
                	else {
                		this_sector = this->other_sector;
                		other_sector = this->this_sector;
                		v1 = this->coordinates_.v2;
                		v2 = this->coordinates_.v1;
                		lower_texture = this->lower_texture[1];
                		xoffset = this->offsets[2];
                		yoffset = this->offsets[3];
                		isTexture = wall_tests.lower_wall[1];
                	}
                    glBindTexture(GL_TEXTURE_2D, lower_texture);
                    GLint tex_w, tex_h;
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_w);
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_h);

                    float wall_height = (this_sector->floor_height - other_sector->floor_height) * TEX_MULTIPLIER;

                    // Pegging for lower: if LowerUnpegged, align from top; else from bottom (adjust yoffset accordingly)
                    int16_t effective_yoffset = yoffset;
                    if (flags_.LowerUnpegged) {
                        // Peg to top: offset from higher floor down
                        effective_yoffset += (this_sector->floor_height - other_sector->floor_height);
                    } else {
                        // Peg to bottom (default): offset from lower floor up
                        // No adjustment needed beyond base offset
                    }

                    float u_left = static_cast<float>(xoffset) / tex_w;
                    float u_right = u_left + wall_len / tex_w;
                    float v_bottom = static_cast<float>(effective_yoffset) / tex_h;  // at top of part
                    float v_top = v_bottom + wall_height / tex_h;

                    glBegin(GL_QUADS);
                    glTexCoord2f(u_left, v_bottom);    glVertex3s(v1.x, this_sector->floor_height, -v1.y);
                    glTexCoord2f(u_right, v_bottom);   glVertex3s(v2.x, this_sector->floor_height, -v2.y);
                    glTexCoord2f(u_right, v_top);      glVertex3s(v2.x, other_sector->floor_height, -v2.y);
                    glTexCoord2f(u_left, v_top);       glVertex3s(v1.x, other_sector->floor_height, -v1.y);
                    glEnd();
                }

                // Render upper if this ceil < other ceil and texture assigned
                if (wall_tests.upper_wall[0] || wall_tests.upper_wall[1]) {

                	svec2 v1,v2;
                	Sector* this_sector, *other_sector;
                	GLuint upper_texture;
                	int16_t xoffset, yoffset;
                	if (this->this_sector->ceil_height > this->other_sector->ceil_height) {
                		this_sector = this->this_sector;
                		other_sector = this->other_sector;
                		v1 = this->coordinates_.v1;
                		v2 = this->coordinates_.v2;
                		upper_texture =this->upper_texture[0];
                		xoffset = this->offsets[0];
                		yoffset = this->offsets[1];
                	}
                	else {
                		this_sector = this->other_sector;
                		other_sector = this->this_sector;
                		v1 = this->coordinates_.v2;
                		v2 = this->coordinates_.v1;
                		upper_texture = this->upper_texture[1];
                		xoffset = this->offsets[2];
                		yoffset = this->offsets[3];
                	}

                    glBindTexture(GL_TEXTURE_2D, upper_texture);
                    GLint tex_w, tex_h;
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_w);
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_h);

                    float wall_height = (other_sector->ceil_height - this_sector->ceil_height) * TEX_MULTIPLIER;

                    // Pegging for upper: if UpperUnpegged, align from bottom; else from top (default)
                    int16_t effective_yoffset = yoffset;
                    if (flags_.UpperUnpegged) {
                        // Peg to bottom: offset from lower ceil up
                        // No adjustment (starts from bottom)
                    } else {
                        // Peg to top (default): offset from higher ceil down
                        effective_yoffset += (other_sector->ceil_height - this_sector->ceil_height);
                    }

                    float u_left = static_cast<float>(xoffset) / tex_w;
                    float u_right = u_left + wall_len / tex_w;
                    float v_bottom = static_cast<float>(effective_yoffset) / tex_h;  // at top of part
                    float v_top = v_bottom + wall_height / tex_h;

                    glBegin(GL_QUADS);
                    glTexCoord2f(u_left, v_bottom);    glVertex3s(v1.x, other_sector->ceil_height, -v1.y);
                    glTexCoord2f(u_right, v_bottom);   glVertex3s(v2.x, other_sector->ceil_height, -v2.y);
                    glTexCoord2f(u_right, v_top);      glVertex3s(v2.x, this_sector->ceil_height, -v2.y);
                    glTexCoord2f(u_left, v_top);       glVertex3s(v1.x, this_sector->ceil_height, -v1.y);
                    glEnd();
                }

                // Render middle if texture assigned (e.g., transparent or fence)
                if (wall_tests.middle_wall[0] || wall_tests.middle_wall[1]) {
	                int16_t z_high = std::min(this_sector->ceil_height, other_sector->ceil_height);
	                int16_t z_low = std::max(this_sector->floor_height, other_sector->floor_height);
	                if (z_high <= z_low) return; // No height, skip

	                // Handle both sides of the wall
	                for (int side = 0; side < 2; side++) {
		                if (!wall_tests.middle_wall[side]) continue;

		                svec2 v1, v2;
		                GLuint texture;
		                int16_t xoffset, yoffset;

		                if (side == 0) {
			                // First side
			                v1 = coordinates_.v1;
			                v2 = coordinates_.v2;
			                texture = middle_texture[0];
			                xoffset = offsets[0];
			                yoffset = offsets[1];
		                } else {
			                // Second side (reversed vertices)
			                v1 = coordinates_.v2;
			                v2 = coordinates_.v1;
			                texture = middle_texture[1];
			                xoffset = offsets[2];
			                yoffset = offsets[3];
		                }

		                glBindTexture(GL_TEXTURE_2D, texture);
		                GLint tex_w, tex_h;
		                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_w);
		                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_h);

		                float wall_height = (z_high - z_low) * TEX_MULTIPLIER;

		                // Middle typically no pegging flags, but can add if needed
		                float u_left = static_cast<float>(xoffset) / tex_w;
		                float u_right = u_left + wall_len / tex_w;
		                float v_bottom = static_cast<float>(yoffset) / tex_h;
		                float v_top = v_bottom + wall_height / tex_h;

		                glBegin(GL_QUADS);
		                glTexCoord2f(u_left, v_bottom);
		                glVertex3s(v1.x, z_high, -v1.y);
		                glTexCoord2f(u_right, v_bottom);
		                glVertex3s(v2.x, z_high, -v2.y);
		                glTexCoord2f(u_right, v_top);
		                glVertex3s(v2.x, z_low, -v2.y);
		                glTexCoord2f(u_left, v_top);
		                glVertex3s(v1.x, z_low, -v1.y);
		                glEnd();
	                }
                }
            }
        }

		bool AllowWalkThrough(const Sector* start) const {
			if (other_sector == nullptr || flags_.Impassible)
				return false;

			const auto t_sec = start == this_sector ? this_sector : other_sector;
			const auto o_sec = start == this_sector ? other_sector : this_sector;
			const auto res = t_sec->floor_height+20 < o_sec->floor_height ||
				o_sec->ceil_height - o_sec->floor_height < 56;
			return !res;
		}

		Sector* getOther(Sector* current) const {
			return other_sector == current ? this_sector : other_sector;
		}

		bool isInCoords(fvec3 point) {
			const float threshold = 1.0f;
			return point.x >= std::min(coordinates_.v1.x, coordinates_.v2.x) - threshold
			       && point.x <= std::max(coordinates_.v1.x, coordinates_.v2.x) + threshold
			       && point.z >= std::min(coordinates_.v1.y, coordinates_.v2.y) - threshold
			       && point.z <= std::max(coordinates_.v1.y, coordinates_.v2.y) + threshold
			       && point.y >= this_sector->floor_height - threshold
			       && point.y <= this_sector->ceil_height + threshold;
		}

		void setFlag(uint16_t flag) {
			memcpy(&flags_, &flag, sizeof(uint16_t));
		}
	};

	class RayCaster{
	public:
		enum RayType {
			Interaction,
			Shot,
			Walk,
			WalkSide,
		};

		static float PlaneIntersectionDistance(Wall* wall, fvec3 vector, svec3 startingPoint, float maxDist) {
			fvec3 planePoint = wall->getPlanePoint();
			fvec3 normal = wall->getPlaneNormal();
			float bottom = normal.dot(vector);
			float top = (planePoint-fvec3(startingPoint)).dot(normal);

			if (bottom == 0)
				return SHRT_MAX;

			float distance = top/bottom;

			if (distance > maxDist || distance < 0)
				return SHRT_MAX;
			fvec3 intersectionPoint = fvec3(startingPoint)+(vector.normalized()*distance);

			if (wall->Intersects(intersectionPoint))
				return distance;
			return SHRT_MAX;
		}

		static float CalculateVectorCeilDist(fvec3 normalizedVector, Sector* sector, svec3 startingPoint) {
			float ceilHeight = floor(sector->ceil_height);
			float dist = (ceilHeight - startingPoint.y)/normalizedVector.y;
			return dist;
		}

		static float CalculateVectorFloorDist(fvec3 normalizedVector, Sector* sector, svec3 startingPoint) {
			float ceilHeight = floor(sector->floor_height);
			float dist = (ceilHeight - startingPoint.y)/normalizedVector.y;
			return dist;
		}

		static fvec3 PerformRayCast(fvec3& vector, Sector*& currentSector, svec3 startingPoint, RayType rayType, bool& target_hit) {
			fvec3 normalizedVector = vector.normalized();
			float vectorLength = vector.length();

			Wall* intersectionWall;
			float intersectionDist = SHRT_MAX;
			for (auto wall : currentSector->GetWalls()) {
				auto planeDist = PlaneIntersectionDistance(wall, vector, startingPoint, 500.);
				if (planeDist < intersectionDist) {
					intersectionDist = planeDist;
					intersectionWall = wall;
				}
			}

			if (true) { //rayType == Walk || rayType ==WalkSide) {
				//printf("[inf] Intersection Dist: %f\n", intersectionDist);
				fvec3 res = {};
				fvec3 sp = fvec3(startingPoint);
				fvec3 vec = fvec3(vector);
				auto thresholdDistance = rayType == WalkSide ? 1. : 17.;
				if (intersectionDist < SHRT_MAX) {
					//printf("%d, %f\n", currentSector->id, intersectionDist);
				}
				if (intersectionDist > vectorLength + thresholdDistance) {
					printf("1\n");
					res = vec + sp;
					vector = fvec3(0,0,0);
				}
				else {
					if (intersectionWall->AllowWalkThrough(currentSector)) {
						printf("2\n");
						res = sp + normalizedVector*(intersectionDist+1);
						vector = vector - normalizedVector*(intersectionDist-1);
						currentSector = intersectionWall->getOther(currentSector);
					}
					else {
						printf("3\n");
						res = sp + normalizedVector*(intersectionDist-thresholdDistance);
						vector = fvec3(0,0,0);
					}
				}

				return fvec3(res.x, res.y,res.z);
			}

			else {
				//Calculate whether ray will cross the ceil or floor before any plane.
				auto floorCeilIntersection = std::max(
					CalculateVectorCeilDist(normalizedVector,currentSector, startingPoint),
					CalculateVectorFloorDist(normalizedVector, currentSector, startingPoint)
					);

				if (intersectionDist > vectorLength && floorCeilIntersection > vectorLength) {
					auto restVector = fvec3(0,0,0);
					vector = restVector;
					target_hit = false;
					return (fvec3)startingPoint + fvec3(round(vector.x), round(vector.y), round(vector.z));
				}

				if (floorCeilIntersection < intersectionDist) {
					auto distVector = normalizedVector * floorCeilIntersection;
					target_hit = true;
					vector =  distVector;
					return (fvec3)startingPoint + fvec3(round(distVector.x), round(distVector.y), round(distVector.z));
				}

				auto distVector = normalizedVector * intersectionDist;
				auto restVector = normalizedVector * (vectorLength - intersectionDist);
				target_hit = false;
				vector = restVector;
				return (fvec3)startingPoint + fvec3(round(distVector.x), round(distVector.y), round(distVector.z));
			}


			return {0,0,0};
		}
	};

    class Map{
		public:
			std::vector<Sector> sectors;
    		std::vector<Wall*> walls;
    		std::vector<Entity*> entities;
			svec3 player_start = {0,0,0};
			uint16_t player_start_angle;
			TexBinder* texture_binder;
    		fvec2 last_player_pos{};

			Map() {
			  texture_binder = new TexBinder();
			}

    		void Render() {
				glEnable(GL_TEXTURE_2D);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				for (auto& wall : walls) {
					wall->Render();
				}


				for (auto& sector : sectors) {
					sector.Render();
				}

				for (auto& entity : entities) {
					entity->Render(last_player_pos);
				}
				glDisable(GL_BLEND);
				glDisable(GL_TEXTURE_2D);
			}

    		void Update(double deltaTime) {
				for (auto& entity : entities) {
					entity->Update(deltaTime);
				}
			}

    		Sector* getPlayerSector(svec2 pos, Sector* previousSector) {
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

			void HandleMovement(fvec3& move, fvec3& player_pos, Sector*& currentSector) {
				bool target_hit;
				fvec3 player_pos_save;
				while (!move.is_zero()) {
					//todo glicz przy wchodzeniu do ściany
					// printf("stop\n");

					fvec3 p_pos = fvec3(round(player_pos.x), round(player_pos.y), round(player_pos.z));
					player_pos_save = player_pos;
					fvec3 perpendicular = fvec3{-move.z, 0, move.x};
					perpendicular = perpendicular.normalized()*16.;

					fvec3 p_pos_2 = p_pos + perpendicular;
					fvec3 p_pos_3 = p_pos - perpendicular;
					fvec3 mv1 = move, mv2 = move, mv3=move;
					auto pos2Sector = getPlayerSector({static_cast<short>(p_pos_2.x),static_cast<short>(p_pos_2.z)}, currentSector);
					auto pos3Sector = getPlayerSector({static_cast<short>(p_pos_3.x),static_cast<short>(p_pos_3.z)}, currentSector);

					p_pos = RayCaster::PerformRayCast(mv1, currentSector, (svec3)p_pos, RayCaster::Walk, target_hit);
					if (pos2Sector) {
						p_pos_2 = RayCaster::PerformRayCast(mv2, pos2Sector, (svec3)p_pos_2, RayCaster::WalkSide, target_hit);
					}
					if (pos3Sector) {
						p_pos_3 = RayCaster::PerformRayCast(mv3, pos3Sector, (svec3)p_pos_3, RayCaster::WalkSide, target_hit);
					}

					p_pos_2 =p_pos_2 - perpendicular;
					p_pos_3 =p_pos_3 + perpendicular;

					float dist_1 = (player_pos - p_pos).length();
					float dist_2 = (player_pos - p_pos_2).length();
					float dist_3 = (player_pos - p_pos_3).length();

					if (dist_1 > dist_2 && dist_1 > dist_3) {
						player_pos = p_pos;
						move = mv1;
					}
					else if (dist_2 > dist_3) {
						player_pos = p_pos_2;
						move = mv2;
					}
					else {
						player_pos = p_pos_3;
						move = mv3;
					}
					svec3 p_pos_s = (svec3)player_pos;
					auto next_sector = getPlayerSector({p_pos_s.x, p_pos_s.z}, currentSector);
					if (next_sector == nullptr)
						player_pos = player_pos_save;
					else
						currentSector = next_sector;
				}
				last_player_pos = fvec2(player_pos.x, player_pos.z);
			}



    		~Map() {
				for (auto& wall : walls) {
					delete wall;
				}
			}
    };
};
#endif //NEW_MODELS_H
