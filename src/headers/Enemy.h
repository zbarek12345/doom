#ifndef HERETIC_ENEMY_H
#define HERETIC_ENEMY_H

#include "Entity.h"
#include "TexBinder.h"
#include "texture.h"

//kolejnosc zgodna z opisem ktory sam pisales
enum class EnemyState {
    Idle = 0,
    Chase,
    Melee,
    Projectile,
    Pain,
    Death,
    Rescoured,
    Count
};

struct EnemyFrameLoader {
    char let;   //litera sekwencji: A,B,C...
    double time;//czas trwania tej klatki (np. w ticach)
};

struct EnemyFrame {
    gl_texture angles[8]; //0..7 = doom 1..8
    bool mirror[8];       //czy odbic w poziomie dla danego kata
    double time;
};


struct EnemyInitiator {
    std::string base_texture_name; //np "TROO"
    uint16_t health;
    uint16_t speed;
    double reactionTime;
    float painChance;
    bool blocks;
    EntityPosType pos_type;
    uint16_t radius;

    //Idle, Chase, Melee, Projectile, Pain, Death, Rescoured
    std::vector<EnemyFrameLoader> loaders[(int)EnemyState::Count];

    EnemyInitiator(std::string base, uint16_t hp, uint16_t spd, double react, float pain, bool blocks, EntityPosType pos_type, uint16_t radius,
                   std::vector<EnemyFrameLoader> idle,
                   std::vector<EnemyFrameLoader> chase,
                   std::vector<EnemyFrameLoader> melee,
                   std::vector<EnemyFrameLoader> projectile,
                   std::vector<EnemyFrameLoader> painSeq,
                   std::vector<EnemyFrameLoader> death,
                   std::vector<EnemyFrameLoader> rescoured)
            : base_texture_name(std::move(base)),
              health(hp),
              speed(spd),
              reactionTime(react),
              painChance(pain),
              blocks(blocks),
              pos_type(pos_type),
              radius(radius)
    {
        loaders[(int)EnemyState::Idle]       = std::move(idle);
        loaders[(int)EnemyState::Chase]      = std::move(chase);
        loaders[(int)EnemyState::Melee]      = std::move(melee);
        loaders[(int)EnemyState::Projectile] = std::move(projectile);
        loaders[(int)EnemyState::Pain]       = std::move(painSeq);
        loaders[(int)EnemyState::Death]      = std::move(death);
        loaders[(int)EnemyState::Rescoured]        = std::move(rescoured);
    }
};

class Enemy: public Entity {
    uint16_t health = 100;
    float painChance = 0.05f;
    uint16_t speed = 0;
    double reactionTime = 0.0;

    EnemyState currentState = EnemyState::Chase;

    //animacje per stan
    std::vector<EnemyFrame> frames[(int)EnemyState::Count];
    double stateTimer = 0.0;
    size_t currentFrame = 0;

    double ticToSeconds = 1.0 / 35.0; //klasyczne doomowe tics

public:
    Enemy(svec2 position, const EnemyInitiator& init);

    void InitAnimations(TexBinder* tb, const EnemyInitiator& init);

    void SetState(EnemyState st);

    void Update(double deltaTime) override;

    void bindTextures(std::vector<gl_texture>& textures) override {} //nieuzywane dla Enemy

    void Render(fvec2 playerPosition) const override;
};

//definicja danych dla Impa, jak Pistol/Shotgun
extern const EnemyInitiator ImpInitiator;

#define Imp(pos) Enemy(pos, ImpInitiator)

#endif //HERETIC_ENEMY_H
