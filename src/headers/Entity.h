//
// Created by Wiktor on 15.11.2025.
//

#ifndef DOOM_ENTITY_H
#define DOOM_ENTITY_H
#include "vec2.h"
#include <GL/gl.h>

enum class EntityPosType{
	Floor,
	Floating,
	Ceiling
};

class Entity {
protected:
	svec2 position;
	svec2 size;
	GLuint texId;
	svec2 h_limits;
	int16_t start_height;
	EntityPosType pos_type;

public:
	virtual ~Entity() = default;

	Entity(svec2 position, svec2 size, GLuint texId, EntityPosType pos_type = EntityPosType::Floor);

	virtual void Update(double deltaTime);

	void Render(fvec2 playerPosition) const;

	void SetLimits(svec2 limits);
};

#define LampEntity(pos, texId) Entity(pos, svec2(23, 48), texId, EntityPosType::Floor)
#define SpaceShipEntity(pos, texId) Entity(pos, svec2(36, 128), texId, EntityPosType::Floor)
#endif //DOOM_ENTITY_H