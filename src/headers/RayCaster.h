//
// Created by Wiktor on 22.11.2025.
//

#ifndef DOOM_RAYCASTER_H
#define DOOM_RAYCASTER_H
#include "Entity.h"
#include "new_models.h"
#include "Player.h"
#include "Wall.h"

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

		static bool RayInfiniteCylinderIntersection(fvec3 origin, fvec3 dir, fvec3 cylCenter, float radius, float& t)
		{
			fvec2 o = fvec2(origin.x, origin.z);
			fvec2 d = fvec2(dir.x,   dir.z).normalized();
			fvec2 c = fvec2(cylCenter.x, cylCenter.z);

			float a = d.dot(d);
			float b = 2 * o.dot(d) - 2 * c.dot(d);
			float c_val = o.dot(o) + c.dot(c) - 2 * o.dot(c) - radius*radius;

			float discriminant = b*b - 4*a*c_val;
			if (discriminant < 0) return false;

			float sqrtD = std::sqrt(discriminant);
			float t1 = (-b - sqrtD) / (2*a);
			float t2 = (-b + sqrtD) / (2*a);

			t = (t1 > 0) ? t1 : t2;
			return (t > 0);
		}

		// Finite height cylinder
		static bool RayCylinderIntersection(fvec3 ro, fvec3 rd, fvec3 center, float r,
									 float bottom, float top, float& tOut, fvec3& hitPos)
		{
			fvec2 o = fvec2(ro.x, ro.z);
			fvec2 d = fvec2(rd.x, rd.z);
			float len = d.length();
			if (len < 1e-6) return false;
			d /= len;

			fvec2 c = fvec2(center.x, center.z);

			float a = d.dot(d);
			float b = 2 * (o.dot(d) - c.dot(d));
			float c_val = o.dot(o) + c.dot(c) - 2*o.dot(c) - r*r;

			float disc = b*b - 4*a*c_val;
			if (disc < 0) return false;

			float sqrtDisc = std::sqrt(disc);
			float t1 = (-b - sqrtDisc) / (2*a);
			float t2 = (-b + sqrtDisc) / (2*a);

			float t = (t1 > 0.001f) ? t1 : t2;
			if (t < 0.001f) return false;

			fvec3 p = ro + rd * (t * len); // because we normalized horizontal part only
			if (p.y >= bottom && p.y <= top)
			{
				tOut = t * len;
				hitPos = p;
				return true;
			}
			return false;
		}


		enum class RayCastResultType{
			Wall,
			Flat,
			Entity,
			Player,
			Monster
		};

		struct RayCastResult {
			RayCastResultType type;
			bool hit;
			double distance;
			void* target;
		};

		struct RayCastResultLess {
			bool operator()(const RayCastResult& lhs, const RayCastResult& rhs) const {
				return lhs.distance < rhs.distance;
			}
		};

		static fvec3 PerformRayCast(
		    fvec3 direction,           // input direction vector (not necessarily normalized)
		    Sector*& currentSector,    // starting sector (may change if portal crossed)
		    fvec3 origin,              // ray origin point
		    RayType rayType,           // e.g. RAY_WEAPON, RAY_SIGHT, etc. (you can use for special rules)
		    bool& target_hit,
		    void* ignore// output: true if we actually hit a blocking target
		) {
		    target_hit = false;
		    std::set<RayCastResult, RayCastResultLess> hits;

		    fvec3 dir = direction.normalized();
		    float remainingDist = direction.length();
		    fvec3 pos = origin;

		    while (remainingDist > 0.01f) // prevent infinite loops and floating-point creep
		    {
		        RayCastResult closest;
		        closest.hit = false;
		        closest.distance = remainingDist; // something bigger than remaining

		        // ------------------------------------------------------------------
		        // 1. Wall / Portal intersections in current sector
		        // ------------------------------------------------------------------
		        float wallDist = remainingDist;
		        Wall* hitWall = nullptr;
		        Sector* nextSector = nullptr;

		        for (Wall* wall : currentSector->GetWalls())
		        {
		            // Skip backfaces or non-solid walls depending on rayType if needed
		            //if (!wall->IsSolid() && !wall->portal) continue;

		            float dist = PlaneIntersectionDistance(wall, dir, pos, remainingDist);
		            if (dist >= 0 && dist < wallDist)
		            {
		                wallDist = dist;
		                hitWall = wall;
		                if (wall->AllowBulletThrough(currentSector, 0))
		                    nextSector = wall->getOther(currentSector);
		                else
		                    nextSector = nullptr;
		            }
		        }

		        if (hitWall)
		        {
		            RayCastResult r;
		        	r.type = RayCastResultType::Wall;
		            r.hit = true;
		            r.distance = wallDist;
		            r.target = hitWall;
		            hits.insert(r);
		        }

		        // ------------------------------------------------------------------
		        // 2. Floor and Ceiling (optional, only if ray can hit them)
		        // ------------------------------------------------------------------
		            // Floor
	            float floorY = currentSector->floor_height;
	            if (dir.y < 0) // looking down
	            {
	                float t = (floorY - pos.y) / dir.y;
	                if (t > 0 && t < remainingDist)
	                {
	                    RayCastResult r{ RayCastResultType::Flat, true, t, nullptr };
	                    hits.insert(r);
	                    if (t < closest.distance) closest = r;
	                }
	            }

	            // Ceiling
	            float ceilY = currentSector->ceil_height;
	            if (dir.y > 0) // looking up
	            {
	                float t = (ceilY - pos.y) / dir.y;
	                if (t > 0 && t < remainingDist)
	                {
	                    RayCastResult r{ RayCastResultType::Flat, true, t, nullptr };
	                    hits.insert(r);
	                    if (t < closest.distance) closest = r;
	                }
	            }

		        // ------------------------------------------------------------------
		        // 3. Player (special case – finite height cylinder)
		        // ------------------------------------------------------------------
	            fvec3 playerPos = Player::GetPosition();
	            float playerHeight = currentSector->floor_height + 56.0f;
	            float playerEye = playerPos.y; // assuming pos.y is eye height already? adjust if needed

	            float radius = 16.;

	            float t;
	            fvec3 hitPoint;
	            if (ignore != Player::GetInstance() && RayCylinderIntersection(pos, dir, playerPos, radius, playerEye - 56.0f, playerEye, t, hitPoint))
	            {

	                if (t < remainingDist)
	                {
	                    RayCastResult r{ RayCastResultType::Player, true, t, nullptr};
	                    hits.insert(r);
	                    if (t < closest.distance) closest = r;
	                }
	            }

		        // ------------------------------------------------------------------
		        // 4. Entities in current sector (cylinders)
		        // ------------------------------------------------------------------
		        for (Entity* ent : currentSector->entities) // assume sector has entity list
		        {
		           // if (!ent || !ent->active) continue;

		            svec2 epos2 = ent->getPosition();
		            fvec3 epos(epos2.x, 0, epos2.y); // assuming height center
		            float radius = ent->getWidth();

		            // Infinite height version (classic Doom things)
		            bool infiniteHeight = true;

		            float t;
	                if (ignore != ent && RayInfiniteCylinderIntersection(pos, dir, epos, radius, t))
	                {
	                    if (t > 0 && t < remainingDist)
	                    {
	                        RayCastResult r{ RayCastResultType::Entity, true, t, ent };
	                        hits.insert(r);
	                        if (t < closest.distance) closest = r;
	                    }
	                }
		        }

		    	if (hits.empty()) {
		    		pos += remainingDist*dir;
		    		break;
		    	}
		        // ------------------------------------------------------------------
		        // Now process hits in order of distance
		        // ------------------------------------------------------------------
		        for (const auto& hit : hits)
		        {
		            if (!hit.hit) continue;

		            // Advance ray to just before this hit
		            fvec3 hitPos = pos + dir * (hit.distance - 0.001f);
		            remainingDist -= hit.distance;

		            // Wall hit
		            if (hit.type == RayCastResultType::Wall)
		            {
		            	auto* wall = static_cast<Wall*>(hit.target);
		                if (wall->AllowBulletThrough(currentSector,0) && remainingDist > 0.01f)
		                {
		                    // Cross portal
		                    currentSector = wall->getOther(currentSector);
		                    pos = hitPos + dir * 0.6f;
		                    break; // restart loop in new sector
		                }
		                else
		                {
		                    // Solid wall → definitive blocking hit
		                    target_hit = true;
		                    return pos + dir * hit.distance; // exact hit point
		                }
		            }
		            // Player hit
		            if (hit.type == RayCastResultType::Player)
		            {
		            	//Player.Eat(Projectile);
		                target_hit = true;
		                return pos + dir * hit.distance;
		            }
		            // Entity hit
		        	if (hit.type == RayCastResultType::Entity)
		            {
		            	Entity* ent = static_cast<Entity*>(hit.target);
		                if (ent->Blocks()) // or flags & MF_SOLID, etc.
		                {
		                    target_hit = true;
		                    return pos + dir * hit.distance;
		                }
		                else
		                {
		                    // Non-blocking → continue past it
		                    pos = hitPos;
		                	continue;
		                }
		            }
		            // Floor/ceiling hit (no target pointer)
		            if (hit.type == RayCastResultType::Flat)
		            {
		                target_hit = true;
		                return pos + dir * hit.distance;
		            }
		        }

		        // If no blocking hit was found in this sector and no portal crossed
		        if (hits.empty() || closest.distance > remainingDist)
		        {
		            // Ray goes into void
		            return pos + dir * remainingDist;
		        }
		    }

		    // Ray exhausted
		    return pos;
		}
	};

}


#endif //DOOM_RAYCASTER_H