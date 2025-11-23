//
// Created by Wiktor on 22.11.2025.
//

#ifndef DOOM_WALL_H
#define DOOM_WALL_H
#include "Sector.h"
#include "texture.h"
#include <cstring>

namespace NewModels {
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

		gl_texture lower_texture[2];
		gl_texture middle_texture[2];
		gl_texture upper_texture[2];

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
			memset(offsets, 0, sizeof(uint16_t)*4);
			special_type = 0;
		}

		void setCoordinates(svec2 v1, svec2 v2) {
			coordinates_.v1 = v1;
			coordinates_.v2 = v2;
		}

		void assignLowerTexture(int id, gl_texture texture) {
			lower_texture[id] = texture;
			wall_tests.lower_wall[id] = true;
		}

		void assignMiddleTexture(int id, gl_texture texture) {
			middle_texture[id] = texture;
			wall_tests.middle_wall[id] = true;
		}

		void assignUpperTexture(int id, gl_texture texture) {
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

		uint16_t getSpecialType() const {
			return special_type;
		}

		coordinates getCoordinates() {
			return coordinates_;
		}

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

            float wall_len = std::sqrt(std::pow(v2.x - v1.x, 2) + std::pow(v2.y - v1.y, 2));

            if (this_sector == nullptr || other_sector == nullptr) {
                // One-sided: render middle as solid wall if texture assigned
                if (!wall_tests.middle_wall) return;
            	auto this_tex = &middle_texture[0];
                glBindTexture(GL_TEXTURE_2D, this_tex->texture_id);

                float wall_height = (this_sector->ceil_height - this_sector->floor_height);

                // Basic texture coord calculation (add pegging logic here if needed)
                float u_left = static_cast<float>(offsets[0]) / this_tex->w;
                float u_right = u_left + wall_len / this_tex->w;
                float v_bottom = static_cast<float>(offsets[1]) / this_tex->h;  // v_bottom at top of wall
                float v_top = v_bottom + wall_height / this_tex->h;  // v_top at bottom of wall

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
                	gl_texture* lower_texture;
                	bool isTexture;
                	int16_t xoffset, yoffset;
                	bool reverse = false;
                	if (this->this_sector->floor_height < this->other_sector->floor_height) {
                		this_sector = this->this_sector;
                		other_sector = this->other_sector;
                		v1 = this->coordinates_.v1;
                		v2 = this->coordinates_.v2;
                		lower_texture = &this->lower_texture[0];
                		xoffset = this->offsets[0];
                		yoffset = this->offsets[1];
                		isTexture = wall_tests.lower_wall[0];
                	}
                	else {
                		reverse = true;
                		this_sector = this->other_sector;
                		other_sector = this->this_sector;
                		v1 = this->coordinates_.v2;
                		v2 = this->coordinates_.v1;
                		lower_texture = &this->lower_texture[1];
                		xoffset = this->offsets[2];
                		yoffset = this->offsets[3];
                		isTexture = wall_tests.lower_wall[1];
                	}
                    glBindTexture(GL_TEXTURE_2D, lower_texture->texture_id);

                    float wall_height = (this_sector->floor_height - other_sector->floor_height);

                    // Pegging for lower: if LowerUnpegged, align from top; else from bottom (adjust yoffset accordingly)
                    int16_t effective_yoffset = yoffset;
                    if (flags_.LowerUnpegged) {
                        // Peg to top: offset from higher floor down
                        effective_yoffset += (this_sector->floor_height - other_sector->floor_height);
                    } else {
                        // Peg to bottom (default): offset from lower floor up
                        // No adjustment needed beyond base offset
                    }

                	float u_left = static_cast<float>(offsets[0]) / lower_texture->w;
                	float u_right = u_left + wall_len / lower_texture->w;
                	float v_bottom = static_cast<float>(offsets[1]) / lower_texture->h;  // v_bottom at top of wall
                	float v_top = v_bottom + wall_height / lower_texture->h;  // v_top at bottom of wall
					if (reverse) {
						std::swap(u_left, u_right);
					}
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
                	gl_texture* upper_texture;
                	int16_t xoffset, yoffset;
                	if (this->this_sector->ceil_height > this->other_sector->ceil_height) {
                		this_sector = this->this_sector;
                		other_sector = this->other_sector;
                		v1 = this->coordinates_.v1;
                		v2 = this->coordinates_.v2;
                		upper_texture = &this->upper_texture[0];
                		xoffset = this->offsets[0];
                		yoffset = this->offsets[1];
                	}
                	else {
                		this_sector = this->other_sector;
                		other_sector = this->this_sector;
                		v1 = this->coordinates_.v2;
                		v2 = this->coordinates_.v1;
                		upper_texture = &this->upper_texture[1];
                		xoffset = this->offsets[2];
                		yoffset = this->offsets[3];
                	}

                    glBindTexture(GL_TEXTURE_2D, upper_texture->texture_id);

                    float wall_height = (other_sector->ceil_height - this_sector->ceil_height);

                    // Pegging for upper: if UpperUnpegged, align from bottom; else from top (default)
                	float u_left = static_cast<float>(offsets[0]) / upper_texture->w;
                	float u_right = u_left + wall_len / upper_texture->w;
                	float v_bottom = static_cast<float>(offsets[1]) / upper_texture->h;  // v_bottom at top of wall
                	float v_top = v_bottom + wall_height / upper_texture->h;  // v_top at bottom of wall

					if (this->flags_.UpperUnpegged) {
						auto v_target = ceil(v_top);
						v_bottom += v_target - v_top;
						v_top = v_target;
					}


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
		                gl_texture* texture;
		                int16_t xoffset, yoffset;

		                if (side == 0) {
			                // First side
			                v1 = coordinates_.v1;
			                v2 = coordinates_.v2;
			                texture = &middle_texture[0];
			                xoffset = offsets[0];
			                yoffset = offsets[1];
		                } else {
			                // Second side (reversed vertices)
			                v1 = coordinates_.v2;
			                v2 = coordinates_.v1;
			                texture = &middle_texture[1];
			                xoffset = offsets[2];
			                yoffset = offsets[3];
		                }

		                glBindTexture(GL_TEXTURE_2D, texture->texture_id);

		                float wall_height = (z_high - z_low);

		                // Middle typically no pegging flags, but can add if needed
		                float u_left = static_cast<float>(xoffset) / texture->w;
		                float u_right = u_left + wall_len / texture->w;
		                float v_bottom = static_cast<float>(yoffset) / texture->h;
		                float v_top = v_bottom + wall_height / texture->h;

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
			const auto res = t_sec->floor_height+35 < o_sec->floor_height ||
				o_sec->ceil_height - o_sec->floor_height < 56;
			return !res;
		}

		bool AllowBulletThrough(const Sector* start, uint16_t height) const {
			if (other_sector == nullptr || flags_.Impassible)
				return false;
			return true;
			const auto t_sec = start == this_sector ? this_sector : other_sector;
			const auto o_sec = start == this_sector ? other_sector : this_sector;
		}

		bool IsRight(const Sector* t) const {
			return this_sector == t;
		}

		Sector* getOther(Sector* current) const {
			return other_sector == current ? this_sector : other_sector;
		}

		Sector* GetLeftSector() const {
			return other_sector;
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
}
#endif //DOOM_WALL_H