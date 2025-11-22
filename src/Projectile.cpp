//
// Created by Wiktor on 22.11.2025.
//

#include "headers/Projectile.h"
#include "headers/Map.h"
#define M_PI 3.14159265358979323846

uint16_t Projectile::GetDamage() const {
	return rand()%(max_damage - min_damage) + min_damage;
}

NewModels::Sector * Projectile::GetSector() const {
	return current_sector;
}

bool Projectile::GetDetails(fvec3 &direction, fvec3 &position) const {
	direction = this->direction;
	position = this->position;
	return true;
}

void Projectile::SetSector(NewModels::Sector *sector) {
	this->current_sector = sector;
}

void Projectile::SetPosition(fvec3 position) {
	this->position = position;
}

void * Projectile::GetCaster() const {
	return caster;
}

void Projectile::Update(double deltaTime) {
	NewModels::Map::HandleProjectile(this, speed*deltaTime);
}

Projectile::Projectile(ProjectileType type, const fvec3 &position, const fvec3 &direction, uint16_t speed,
	uint16_t min_damage, uint16_t max_damage, void* caster)
{
	this->position = position;
	this->position.y -= 5;
	this->direction = direction.normalized();
	this->direction.y = - direction.y;
	this->type = type;
	this->speed = speed;
	this->min_damage = min_damage;
	this->max_damage = max_damage;
	this->caster = caster;
	current_sector = NewModels::Map::getPlayerSector({static_cast<short>(position.x), static_cast<short>(position.z)}, nullptr);
}

ProjectileType Projectile::GetType() const {
	return type;
}

BulletProjectile::BulletProjectile(fvec3 position, fvec3 direction, void* caster):Projectile(ProjectileType::BULLET, position, direction, 1200, 5, 15, caster) {}

bool BulletProjectile::HasExplosion(uint16_t &damage, uint16_t &radius) {
	return Projectile::HasExplosion(damage, radius);
}

void BulletProjectile::Render() {
	glPushMatrix();
	glTranslatef(position.x, position.y, -position.z);

	auto dir = direction.normalized();

	float pitch = asinf(-dir.y);                     // note the NEGATIVE!
	float yaw   = atan2f(dir.x, dir.z);

	glRotatef(yaw   * 180/M_PI, 0,1,0);
	glRotatef(pitch * 180/M_PI, 1,0,0);
	// Set bright yellow color
	glColor3f(1.0f, 1.0f, 0.0f);

	// Draw elongated cube
	glBegin(GL_QUADS);
	// Front face
	glVertex3f(-5.f, -5.f, 0.0f);
	glVertex3f(5.f, -5.f, 0.0f);
	glVertex3f(5.f, 5.f, 0.0f);
	glVertex3f(-5.f, 5.f, 0.0f);

	// Back face
	glVertex3f(-0.5f, -0.5f, 10.0f);
	glVertex3f(0.5f, -0.5f, 10.0f);
	glVertex3f(0.5f, 0.5f, 10.0f);
	glVertex3f(-0.5f, 0.5f, 10.0f);

	// Top face
	glVertex3f(-0.5f, 0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 10.0f);
	glVertex3f(-0.5f, 0.5f, 10.0f);

	// Bottom face
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 10.0f);
	glVertex3f(-0.5f, -0.5f, 10.0f);

	// Right face
	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 10.0f);
	glVertex3f(0.5f, -0.5f, 10.0f);

	// Left face
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glVertex3f(-0.5f, 0.5f, 0.0f);
	glVertex3f(-0.5f, 0.5f, 10.0f);
	glVertex3f(-0.5f, -0.5f, 10.0f);
	glEnd();

	glPopMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
}

bool Projectile::HasExplosion(uint16_t &damage, uint16_t &radius) {
	return false;
}




