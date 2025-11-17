//
// Created by Wiktor on 15.11.2025.
//

#ifndef DOOM_ENTITY_H
#define DOOM_ENTITY_H
#include "vec2.h"
#include "texture.h"

enum class EntityPosType{
	Floor,
	Floating,
	Ceiling
};

class Entity {
protected:
	svec2 position;
	uint16_t width;
	gl_texture tex;
	svec2 h_limits;
	int16_t start_height;
	EntityPosType pos_type;

public:
	virtual ~Entity() = default;

	Entity(svec2 position, uint16_t size, gl_texture tex, EntityPosType pos_type = EntityPosType::Floor);

	virtual void Update(double deltaTime);

	void Render(fvec2 playerPosition) const;

	void SetLimits(svec2 limits);
};

#define LampEntity(pos, tex) Entity(pos, 16, tex, EntityPosType::Floor)
#define SpaceShipEntity(pos, tex) Entity(pos, 16, tex, EntityPosType::Floor)
#endif //DOOM_ENTITY_H