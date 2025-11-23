#include "./headers/Enemy.h"


Enemy::Enemy(const svec2 pos, uint16_t width, std::string base_tex_name, std::string tex_sequence, bool blocking, EntityPosType type):
    Entity(pos, width, base_tex_name, tex_sequence, blocking, type){

}
