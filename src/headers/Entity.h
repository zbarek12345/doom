//
// Created by Wiktor on 15.11.2025.
//

#ifndef DOOM_ENTITY_H
#define DOOM_ENTITY_H
#include <vector>

#include "vec2.h"
#include "texture.h"

enum class EntityPosType{
	Floor,
	Floating,
	Ceiling
};

class Entity {
protected:
	bool taken = false;
	svec2 position;
	uint16_t width;
	gl_texture tex;
	svec2 h_limits;
	int16_t start_height;
	EntityPosType pos_type;
	std::string base_texture_name;
	bool blocks = true;
	std::string tex_sequence;

public:
	virtual ~Entity() = default;

	Entity(svec2 position, uint16_t size, std::string base_tex_name, std::string tex_sequence, bool blocks,  EntityPosType pos_type = EntityPosType::Floor);

	virtual void Update(double deltaTime);

	virtual void bindTextures(std::vector<gl_texture>& textures);

	void Render(fvec2 playerPosition) const;

	void SetLimits(svec2 limits);

	virtual bool AllowCollection() const;

	bool Blocks() const;

	virtual void Collect();

	svec2 getPosition() const;

	uint16_t getWidth() const;

	std::string getBaseName();

	std::string getTexSequence();

	bool isToRemove() const;

	///Retrieves position and block treshold/ capture treshold.
	void getPosAndRad(svec2& pos, uint16_t& width) const;
};

#define BloodyMessEntity(pos) Entity(pos, 16, "PLAY", "W", false, EntityPosType::Floor)
#define LampEntity(pos) Entity(pos, 16, "COLU", "A", true, EntityPosType::Floor)
#define SpaceShipEntity(pos) Entity(pos, 16, "ELEC","A", true, EntityPosType::Floor)
#endif //DOOM_ENTITY_H