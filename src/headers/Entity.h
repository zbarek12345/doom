//
// Created by Wiktor on 15.11.2025.
//

#ifndef DOOM_ENTITY_H
#define DOOM_ENTITY_H
#include "vec2.h"
#include <GL/gl.h>

class Entity {
protected:
	svec2 position;
	svec2 size;
	GLuint texId;

public:
	Entity(svec2 position, svec2 size, GLuint texId);

	void Update(double deltaTime);

	void Render(fvec2 playerPosition) const;
};

#endif //DOOM_ENTITY_H