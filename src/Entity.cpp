//
// Created by Wiktor on 15.11.2025.
//
#include "headers/Entity.h"

Entity::Entity(svec2 position, svec2 size, GLuint texId) {
	this->position = position;
	this->size = size;
	this->texId = texId;
}

void Entity::Update(double deltaTime) {

}

void Entity::Render(const fvec2 playerPosition) const {
	fvec2 vect = static_cast<fvec2>(position) - playerPosition;
	auto perp = vect.perpendicular();
	perp.normalize();

	perp*= size.x/2;

	auto lcorn = vect + perp;
	auto rcorn = vect - perp;

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_TRIANGLES);
		glVertex3f(lcorn.x, size.y, lcorn.y); glTexCoord2f(0,0);
		glVertex3f(rcorn.x, size.y ,rcorn.y); glTexCoord2f(1,0);
		glVertex3f(rcorn.x, 0, lcorn.y); glTexCoord2f(1,1);
		glVertex3f(lcorn.x, size.y, lcorn.y); glTexCoord2f(0,0);
		glVertex3f(rcorn.x, 0, rcorn.y); glTexCoord2f(1,1);
		glVertex3f(rcorn.x,0, lcorn.y); glTexCoord2f(0,1);
	glEnd();
}

