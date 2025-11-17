//
// Created by Wiktor on 17.11.2025.
//

#ifndef DOOM_TEXTURE_H
#define DOOM_TEXTURE_H
#include <cstdint>
#include <GL/glew.h>
#include <GL/gl.h>

struct raw_texture {
	uint16_t w,h;
	uint8_t* data;
};

inline void raw_texture_destroy(raw_texture* texture) {
	delete [] texture->data;
}

struct gl_texture {
	uint16_t w,h;
	GLuint texture_id;
};

#endif //DOOM_TEXTURE_H