//
// Created by Wiktor on 22.11.2025.
//

#include "../headers/Sector.h"
#include "../headers/Player.h"

void NewModels::Sector::BindTextureCoords(svec2 coords, gl_texture *texture) const {
	fvec2 diff = {(float)coords.x/(float)texture->w, (float)coords.y/(float)texture->h};
	if (id==45 && !have_print)
		printf(" Tex Coords : (%f %f)\n", diff.x, -diff.y);
	glTexCoord2f(diff.x, -diff.y);
}

void NewModels::Sector::Render() {
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);  // Standard: cull backs for both
	// Ceilings
	glBindTexture(GL_TEXTURE_2D, ceil_texture.texture_id);
	glBegin(GL_TRIANGLES);
	for (auto& line: lines) {
		// REVERSE ORDER for ceil: v3, v2, v1 -> CCW from below
		auto p = nodes[line.v1];
		if (id==45 && !have_print)
			printf("\n Real Coords : (%hd %hd)", p.x, -p.y);
		BindTextureCoords(p, &ceil_texture); glVertex3f(p.x, ceil_height, -p.y);

		p = nodes[line.v2];
		if (id==45 && !have_print)
			printf(" Real Coords : (%hd %hd)", p.x, -p.y);
		BindTextureCoords(p, &ceil_texture); glVertex3f(p.x, ceil_height, -p.y);

		p = nodes[line.v3];
		if (id==45 && !have_print)
			printf(" Real Coords : (%hd %hd)", p.x, -p.y);
		BindTextureCoords(p, &ceil_texture); glVertex3f(p.x, ceil_height, -p.y);
	}
	glEnd();
	// Floors (normal order)
	glBindTexture(GL_TEXTURE_2D, floor_texture.texture_id);
	glBegin(GL_TRIANGLES);  // Move outside loop for efficiency
	for (auto& line: lines) {
		auto p = nodes[line.v3];
		if (id==45 && !have_print)
			printf("\n Real Coords : (%hd %hd)", p.x, -p.y);
		BindTextureCoords(p, &floor_texture); glVertex3f(p.x, floor_height, -p.y);

		p = nodes[line.v2];
		if (id==45 && !have_print)
			printf(" Real Coords : (%hd %hd)", p.x, -p.y);
		BindTextureCoords(p, &floor_texture); glVertex3f(p.x, floor_height, -p.y);

		p = nodes[line.v1];
		if (id==45 && !have_print)
			printf(" Real Coords : (%hd %hd)", p.x, -p.y);
		BindTextureCoords(p, &floor_texture); glVertex3f(p.x, floor_height, -p.y);
	}
	have_print = true;
	glEnd();
	glCullFace(GL_FRONT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	auto player_pos3 = Player::GetPosition();
	fvec2 playerpos = {player_pos3.x, player_pos3.z};
	for (auto& entity: entities)
		entity->Render(playerpos);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
}

void NewModels::Sector::Update(double deltaTime) {
	for (auto& entity : entities) {
		if (entity->isToRemove()) {
			delete entity;
			entities.erase(entity);
		}
		entity->Update(deltaTime);
	}
}

bool NewModels::Sector::isInSector(fvec3 pos) {
	if (pos.x < bounding_box[0].x || pos.x > bounding_box[1].x || pos.y < bounding_box[0].y || pos.y>bounding_box[1].y) return false;

	for (auto& triangle : lines) {
		if (isInTriangle(svec3(nodes[triangle.v1].x,nodes[triangle.v1].y, 0),
		                 svec3(nodes[triangle.v2].x,nodes[triangle.v2].y, 0),
		                 svec3(nodes[triangle.v3].x,nodes[triangle.v3].y, 0), pos)) return true;
	}
	return false;
}

void NewModels::Sector::bindTextures(gl_texture floor_texture, gl_texture ceil_texture) {
	this->floor_texture = floor_texture;
	this->ceil_texture = ceil_texture;
}

std::vector<NewModels::Wall *> & NewModels::Sector::GetWalls() {
	return walls;
}

void NewModels::Sector::bindWall(Wall *wall) {
	walls.push_back(wall);
}
