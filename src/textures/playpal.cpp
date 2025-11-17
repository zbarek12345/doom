//
// Created by Wiktor on 28.10.2025.
//

#include "../headers/playpal.h"

void playpal_t::destroy_palette() {
	delete [] palette;
}

void playpal_t::load_palette(uint8_t *palette) {
	this->palette = palette;
}

color3 playpal_t::get_color(uint8_t index) {
	return {palette[index*3], palette[index*3+1], palette[index*3+2]};
}

bool playpal_t::is_loaded() {
	return palette != nullptr;
}


