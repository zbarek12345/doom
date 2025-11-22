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

#include "Entity.h"
#include "new_models.h"
#include "Projectile.h"
#include "TexBinder.h"
#include "vec2.h"
#include "vec3.h"

///TODO - - TODAY !! (Split this beetween files), Create door actions .
namespace NewModels{
	class Wall;
	struct triangle;
	class Sector;
	class Map;
	class RayCaster;
	class ActionPerformer;

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
};
#endif //NEW_MODELS_H
