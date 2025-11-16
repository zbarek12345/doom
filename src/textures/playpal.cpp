//
// Created by Wiktor on 28.10.2025.
//

#include "playpal.h"

uint8_t* playpal::palette = nullptr;

void playpal::destroy_palette() {
	delete [] palette;
}

void playpal::load_palette(uint8_t *palette) {
	palette = playpal::palette;
}

color3 playpal::get_color(uint8_t index) {
	return {palette[index*3], palette[index*3+1], palette[index*3+2]};
}

bool playpal::is_loaded() {
	return palette != nullptr;
}


