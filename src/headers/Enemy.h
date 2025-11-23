
#ifndef HERETIC_ENEMY_H
#define HERETIC_ENEMY_H
#include <pthread.h>
#include <bits/pthreadtypes.h>

#include "Entity.h"

struct RotationalAnimation {
	gl_texture angles[8];

	///Whether Frame [1],[2],[3] is mirrored;
	bool mirrors[3];
	double frame_time;
};

struct EnemyAnimationLoader {
	std::string base_name;
	///Idle, Chase, Melee(opt.), Projectile(opt.), Pain, Death, Gibbing (opt.)
	std::vector<std::string> texture_sequences[7];
};

class Enemy: public Entity {
	uint16_t health = 100;
	float painChance = 0.05f;
	uint16_t speed;
	double reactionTime;

	Enemy(EnemyAnimationLoader loader,uint16_t health,uint16_t speed,double reactionTime,float painChance);
	///Idle, Chase, Melee(opt.), Projectile(opt.), Pain, Death, Gibbing (opt.)
	std::vector<RotationalAnimation> animations[7];

	///TODO - W ten sposób wczytamy animacje ...
	void LoadAnimations();

	enum class State {
		Idle,
		Chasing,
		Melee,
		Projectile,
		Pain,
		Dying,
		Dead,
		Resurrected,
	};

	public:

};

#endif //HERETIC_ENEMY_H
