//
// Created by Wiktor on 15.11.2025.
//
#include "headers/Entity.h"

Entity::Entity(svec2 position, svec2 size, GLuint texId, EntityPosType pos_type) {
	this->position = position;
	this->size = size;
	this->texId = texId;
	this->pos_type = pos_type;
	start_height = 0;
}

void Entity::Update(double deltaTime) {
	if (pos_type == EntityPosType::Floating) {
		position.y = start_height + sin(deltaTime*10)*10;
	}
}

void Entity::Render(const fvec2 playerPosition) const
{
	fvec2 toEntity = static_cast<fvec2>(position) - playerPosition; // vector from player to entity
	fvec2 right = toEntity.perpendicular().normalized();          // right vector in world XZ

	float halfWidth = size.x;
	fvec2 offset = right * halfWidth;

	// Four corners in world space (X, Z)
	fvec2 bottomLeft  = (fvec2)position - offset;
	fvec2 bottomRight = (fvec2)position + offset;
	fvec2 topLeft     = bottomLeft;
	fvec2 topRight    = bottomRight;

	float bottomY = start_height;
	float topY    = start_height + size.y;

	// Depth is the same for all four vertices (important for correct Z buffer!)
	float depth = toEntity.length(); // or just toEntity.x/toEntity.y depending on convention
	// If your camera looks along -Z and +Z is forward, you might want -toEntity.length() etc.
	// Adjust sign so farther = larger Z or smaller Z depending on your GL projection
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_TRIANGLES);
		// First triangle: bottom-left, bottom-right, top-right
		glTexCoord2f(0, 1); glVertex3f(bottomLeft.x,  bottomY, -bottomLeft.y);   // note: UV (0,1) for bottom
		glTexCoord2f(1, 1); glVertex3f(bottomRight.x, bottomY, -bottomRight.y);
		glTexCoord2f(1, 0); glVertex3f(topRight.x,    topY,    -topRight.y);

		// Second triangle: bottom-left, top-right, top-left
		glTexCoord2f(0, 1); glVertex3f(bottomLeft.x,  bottomY, -bottomLeft.y);
		glTexCoord2f(1, 0); glVertex3f(topRight.x,    topY,    -topRight.y);
		glTexCoord2f(0, 0); glVertex3f(topLeft.x,     topY,    -topLeft.y);

	glEnd();
}

void Entity::SetLimits(svec2 limits) {
	h_limits = limits;
	switch (pos_type) {
		case EntityPosType::Floor:
			start_height = limits.x;
			break;
		case EntityPosType::Ceiling:
			start_height = limits.y-size.y;
			break;
		case EntityPosType::Floating:
			start_height = rand()%(limits.y-size.y-limits.x+1)+limits.x;
			break;
	}
}
