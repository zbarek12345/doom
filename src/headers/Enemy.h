
#ifndef HERETIC_ENEMY_H
#define HERETIC_ENEMY_H
#include <pthread.h>
#include <bits/pthreadtypes.h>

#include "Entity.h"

class Enemy: public Entity {
    public:
    Enemy(svec2 pos, int i, const char * str, const char * text, bool cond, EntityPosType floor);

};

//todo enemy
#define Imp(pos) Enemy(pos, 20, "TROO","ABCD", true, EntityPosType::Floor)
#endif //HERETIC_ENEMY_H
