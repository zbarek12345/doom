//
// Created by Wiktor on 15.11.2025.
//

#ifndef DOOM_ANIMATEDENTITY_H
#define DOOM_ANIMATEDENTITY_H
#include <cstdint>
#include <vector>
#include "Entity.h"


class AnimatedEntity : public Entity {
private:
	std::vector<gl_texture>  textures;
	uint8_t current_ = 0;
	double swap_time=0;
	const double swap_interval = 2e-1;

public:
	AnimatedEntity(svec2 pos, uint16_t width, std::string base_tex_name, std::string tex_sequence, bool blocking, EntityPosType type = EntityPosType::Floor);

	void bindTextures(std::vector<gl_texture>& textures) override;

	void Update(double deltaTime) override;
};

#define BarrelEntity(pos) AnimatedEntity(pos, 16, "BAR1", "AB", true, EntityPosType::Floor)


#endif //DOOM_ANIMATEDENTITY_H