//
// Created by Wiktor on 15.11.2025.
//

#include "headers/AnimatedEntity.h"

AnimatedEntity::AnimatedEntity(const svec2 pos, uint16_t width, std::string base_tex_name, std::string tex_sequence, bool blocking, EntityPosType type):
	Entity(pos, width, base_tex_name, tex_sequence, blocking, type){
	current_ = 0;
}

void AnimatedEntity::bindTextures(std::vector<gl_texture> &textures) {
	current_ = rand()%textures.size();
	this->textures = textures;
	tex = textures[current_];
}

void AnimatedEntity::Update(const double deltaTime) {
	swap_time+=deltaTime;
	if(swap_time>swap_interval){
		swap_time=0;
		current_=(current_+1)%textures.size();
		tex = textures[current_];
	}
}
