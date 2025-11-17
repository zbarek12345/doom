//
// Created by Wiktor on 15.11.2025.
//

#include "headers/AnimatedEntity.h"

AnimatedEntity::AnimatedEntity(const svec2 pos, uint16_t width, std::vector<gl_texture> textures, EntityPosType type):Entity(pos, width, textures[0], type){
	this->textures = textures;
	current_ = 0;
}

void AnimatedEntity::Update(const double deltaTime) {
	swap_time+=deltaTime;
	if(swap_time>swap_interval){
		swap_time=0;
		current_=(current_+1)%textures.size();
		tex = textures[current_];
	}
}