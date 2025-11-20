//
// Created by Wiktor on 15.11.2025.
//
#include "headers/Entity.h"

Entity::Entity(svec2 position, uint16_t size, std::string base_tex_name , std::string tex_sequence, bool blocks,  EntityPosType pos_type) {
	this->position = position;
	this->width = size;
	this->blocks = blocks;
	this->tex_sequence = tex_sequence;
	this->base_texture_name = base_tex_name;
	this->pos_type = pos_type;
	start_height = 0;
}

void Entity::Update(double deltaTime) {
	if (pos_type == EntityPosType::Floating) {
		position.y = start_height + sin(deltaTime*10)*10;
	}
}

void Entity::bindTextures(std::vector<gl_texture> &textures) {
	assert(textures.size() == 1);
	tex = textures[0];
}

void Entity::Render(const fvec2 playerPosition) const
{
	fvec2 toEntity = static_cast<fvec2>(position) - playerPosition; // vector from player to entity
	fvec2 right = toEntity.perpendicular().normalized();          // right vector in world XZ


	float halfWidth = tex.w/2.f;
	fvec2 offset = right * halfWidth;

	// Four corners in world space (X, Z)
	fvec2 bottomLeft  = (fvec2)position - offset;
	fvec2 bottomRight = (fvec2)position + offset;
	fvec2 topLeft     = bottomLeft;
	fvec2 topRight    = bottomRight;

	float bottomY = start_height;
	float topY    = start_height + tex.h;

	// Depth is the same for all four vertices (important for correct Z buffer!)
	float depth = toEntity.length(); // or just toEntity.x/toEntity.y depending on convention
	// If your camera looks along -Z and +Z is forward, you might want -toEntity.length() etc.
	// Adjust sign so farther = larger Z or smaller Z depending on your GL projection
	glBindTexture(GL_TEXTURE_2D, tex.texture_id);
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
			start_height = limits.y-tex.h;
			break;
		case EntityPosType::Floating:
			start_height = rand()%(limits.y-tex.h-limits.x+1)+limits.x;
			break;
	}
}

bool Entity::AllowCollection() const {
	return false;
}

bool Entity::Blocks() const {
	return this->blocks;
}

void Entity::Collect() const {}

svec2 Entity::getPosition() const {
	return position;
}

uint16_t Entity::getWidth() const {
	return width;
}

std::string Entity::getBaseName() {
	return base_texture_name;
}

std::string Entity::getTexSequence() {
	return tex_sequence;
}

void Entity::getPosAndRad(svec2 &pos, uint16_t &width) const {
	pos = position;
	width = this->width;
}
