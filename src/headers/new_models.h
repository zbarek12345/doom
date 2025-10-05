//
// Created by Wiktor on 05.10.2025.
//

#ifndef NEW_MODELS_H
#define NEW_MODELS_H
#include<vector>
#include<stdint.h>
#include <algorithm>

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
        int id;
		int16_t ceil_height, floor_height;
    	std::vector<vec3> ceiling;
        std::vector<vec3> floor;
		std::vector<uint16_t> line;

        std::vector<wall> walls;

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
    };

    class Map{
      public:
        std::vector<Sector> sectors;
    };
};
#endif //NEW_MODELS_H
