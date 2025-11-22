//
// Created by Wiktor on 22.11.2025.
//

#ifndef DOOM_RAYCASTER_H
#define DOOM_RAYCASTER_H
#include "new_models.h"

namespace NewModels {
	class RayCaster{
	public:
		enum RayType {
			Interaction,
			Shot,
			Walk,
			WalkSide,
		};

		static float PlaneIntersectionDistance(Wall* wall, fvec3 vector, fvec3 startingPoint, float maxDist) {
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

		static float CalculateVectorCeilDist(fvec3 normalizedVector, Sector* sector, fvec3 startingPoint) {
			float ceilHeight = floor(sector->ceil_height);
			float dist = (ceilHeight - startingPoint.y)/normalizedVector.y;
			return dist;
		}

		static float CalculateVectorFloorDist(fvec3 normalizedVector, Sector* sector, fvec3 startingPoint) {
			float ceilHeight = floor(sector->floor_height);
			float dist = (ceilHeight - startingPoint.y)/normalizedVector.y;
			return dist;
		}

		//zwraca true jesli odcinek [start, start+move] przecina odcinek [a,b]
		//jesli outPoint != nullptr, wpisze punkt przeciecia
		static bool SegmentMoveIntersect(const fvec2& start, const fvec2& move, const fvec2& a, const fvec2& b, fvec2* outPoint = nullptr)
		{
			const float eps = 1e-6f;

			fvec2 p = start;
			fvec2 r = move;      //wektor odcinka: koniec = p + r
			fvec2 q = a;
			fvec2 s = b - a;     //wektor odcinka ab

			float rxs = r.cross(s);
			if (std::fabs(rxs) < eps) {
				//linie rownolegle lub wspolliniowe -> tu ignorujemy
				return false;
			}

			fvec2 qp = q - p;

			float t = qp.cross(s) / rxs; //parametr na [start,start+move]
			float u = qp.cross(r) / rxs; //parametr na [a,b]

			//t w [0,1] -> na pierwszym odcinku
			//u w [0,1] -> na drugim odcinku
			if (t >= 0.0f && t <= 1.0f &&
				u >= 0.0f && u <= 1.0f)
			{
				if (outPoint) {
					*outPoint = p + r * t;
				}
				return true;
			}

			return false;
		}

		static float check_object_collision(const svec2& obj_pos, uint16_t width, const fvec3& player_pos, const fvec3& move_vec, bool& collides) {
			float R = 16.0f + static_cast<float>(width);
			float ox = static_cast<float>(obj_pos.x);
			float oz = static_cast<float>(obj_pos.y);
			float px = player_pos.x;
			float pz = player_pos.z;
			float vx = move_vec.x;
			float vz = move_vec.z;
			float dx = px - ox;
			float dz = pz - oz;
			float D2 = dx * dx + dz * dz;
			float R2 = R * R;

			if (D2 <= R2) {
				collides = true;
				return 0.0f;
			}

			float a = vx * vx + vz * vz;
			if (a == 0.0f) {
				collides = false;
				return std::numeric_limits<float>::infinity();
			}

			float b = 2.0f * (dx * vx + dz * vz);
			float c = D2 - R2;
			float delta = b * b - 4.0f * a * c;

			if (delta < 0.0f) {
				collides = false;
				return std::numeric_limits<float>::infinity();
			}

			float sqrt_delta = std::sqrt(delta);
			float t1 = (-b - sqrt_delta) / (2.0f * a);

			if (t1 >= 0.0f && t1 <= 1.0f) {
				collides = true;
				return t1 * std::sqrt(a);
			} else {
				collides = false;
				return std::numeric_limits<float>::infinity();
			}
		}

		struct ObjectCollisionResult {
			Entity* entity;
			float distance;
		};

		struct ObjectCollisionResultLess {
			bool operator()(const ObjectCollisionResult& lhs, const ObjectCollisionResult& rhs) const {
				return lhs.distance < rhs.distance;
			}
		};

		static fvec3 MovementRayCast(fvec3& vector, Sector*& currentSector, fvec3 startingPoint, RayType rayType) {
			fvec3 endPoint = startingPoint + vector;

			std::set<ObjectCollisionResult, ObjectCollisionResultLess> collisionResults;
			for (auto& entity : currentSector->entities) {
				bool isCollision;
				float distance = check_object_collision(entity->getPosition(), entity->getWidth()+2., startingPoint, vector, isCollision);
				if (isCollision)
					collisionResults.emplace(ObjectCollisionResult{entity, distance});
			}

			// Check all walls in a given sector.
			for (auto wall : currentSector->GetWalls()) {
				auto coordinateWall = wall->getCoordinates();

				fvec2 wall1 = {static_cast<float>(coordinateWall.v1.x), static_cast<float>(coordinateWall.v1.y)};
				fvec2 wall2 = {static_cast<float>(coordinateWall.v2.x), static_cast<float>(coordinateWall.v2.y)};

				float r = 16;
				// resize wall
				fvec2 dir = wall2-wall1;
				dir.normalize();
				wall1 = wall1 - dir * r;
				wall2 = wall2 + dir * r;
				//resize move
				dir = fvec2{vector.x, vector.z};
				dir += dir.normalized() * r;


				// check if we have a collision with the line
				if (SegmentMoveIntersect(fvec2{startingPoint.x, startingPoint.z}, dir, wall1, wall2)) {
					//check if it is possible to go through the wall
					if (!wall->AllowWalkThrough(currentSector)) {
						vector = fvec3(0, 0, 0);
						return {startingPoint.x, startingPoint.y,startingPoint.z};
					}
				}
			}

			for (auto& collision : collisionResults) {
				if (collision.entity->Blocks()) {
					vector = fvec3(0, 0, 0);
					return startingPoint + vector.normalized()*(collision.distance-0.05);
				}
				if (collision.entity->AllowCollection()) {
					collision.entity->Collect();
					currentSector->entities.erase(collision.entity);
				}
			}

			vector = fvec3(0, 0, 0);
			return endPoint;
		}

		static fvec3 PerformRayCast(fvec3& vector, Sector*& currentSector, fvec3 startingPoint, RayType rayType, bool& target_hit) {
			fvec3 normalizedVector = vector.normalized();
			float vectorLength = vector.length();
			auto intersectionDist = SHRT_MAX;
			Wall* intersectionWall = nullptr;
			for (auto& wall : currentSector->GetWalls()) {
				auto dist = PlaneIntersectionDistance(wall, normalizedVector, startingPoint, vectorLength);
				if (dist < intersectionDist) {
					intersectionDist = dist;
					intersectionWall = wall;
				}
			}

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
			return (fvec3)startingPoint + distVector;

			return {0,0,0};
		}
	};

}


#endif //DOOM_RAYCASTER_H