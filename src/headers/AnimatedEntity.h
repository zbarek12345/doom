//
// Created by Wiktor on 15.11.2025.
//

#ifndef DOOM_ANIMATEDENTITY_H
#define DOOM_ANIMATEDENTITY_H
#include <cstdint>
#include <vector>

#include <GL/gl.h>

#include "Entity.h"


class AnimatedEntity : public Entity {
private:
	std::vector<GLuint>  textures;
	uint8_t current_ = 0;
	double swap_time=0;
	const double swap_interval = 2e-1;

public:
	AnimatedEntity(svec2 pos, svec2 size, std::vector<GLuint> textures, EntityPosType type = EntityPosType::Floor);

	void Update(double deltaTime) override;
};

#define BarrelEntity(pos, textures) AnimatedEntity(pos, svec2(23, 32), textures, EntityPosType::Floor)
#define GlassOfWaterEntity(pos, textures) AnimatedEntity(pos, svec2(14, 18), textures, EntityPosType::Floor)

#endif //DOOM_ANIMATEDENTITY_H