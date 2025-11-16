//
// Created by Wiktor on 15.11.2025.
//

#ifndef DOOM_ANIMATEDENTITY_H
#define DOOM_ANIMATEDENTITY_H
#include <cstdint>
#include <vector>

#include <GL/gl.h>

#include "Entity.h"


class AnimatedEntity : Entity {
private:
	std::vector<GLuint>  textures;
	uint8_t current_ = 0;
	uint32_t swap_time=0;
	const uint32_t swap_interval = 5e2;

public:
	AnimatedEntity(svec2 pos, svec2 size, const std::vector<GLuint> &textures);

	void Update(double deltaTime) override;
};


#endif //DOOM_ANIMATEDENTITY_H