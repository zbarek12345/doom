#include "headers/Enemy.h"

#include <iostream>

#include "headers/TexBinder.h"

static bool isValidTexture(const gl_texture& t) {
    return t.texture_id != 0 && t.w > 0 && t.h > 0;
}

Enemy::Enemy(svec2 position, const EnemyInitiator& init)
    : Entity(position,
             init.radius,
             init.base_texture_name,
             "",               //tex_sequence niepotrzebne dla Enemy
             init.blocks,
             init.pos_type)
{
    health = init.health;
    speed = init.speed;
    reactionTime = init.reactionTime;
    painChance = init.painChance;
}

void Enemy::InitAnimations(TexBinder* tb, const EnemyInitiator& init) {
    //dla kazdego stanu
    for(int s = 0; s < (int)EnemyState::Count; ++s) {
        auto& loaderSeq = init.loaders[s];
        auto& outSeq = frames[s];
        outSeq.clear();

        for(const auto& fl : loaderSeq) {
            //obsluga A1/A2A8 itp jak wczesniej
            gl_texture chosen{};
            bool found = false;
            std::string base = init.base_texture_name;
            char c = fl.let;

            std::string candidates[] = {
                base + c + "1",
                base + c + "2",
                base + c + "2",
                base + c + "3A7",
                base + c + "4A6",
                base + c + "5"
            };

            for(const auto& name : candidates) {
                auto texTry = tb->GetTexture(name.c_str(), TextureType::ItemTexture);
                if(isValidTexture(texTry)) {
                    chosen = texTry;
                    found = true;
                    break;
                }
            }

            if(found) {
                EnemyFrame fr;
                fr.texture = chosen;
                fr.time = fl.time * ticToSeconds;
                outSeq.push_back(fr);
            }
        }
    }

    //ustaw startowa teksture
    auto& idle = frames[(int)EnemyState::Chase];
    if(!idle.empty()) {
        tex = idle[0].texture;
        currentState = EnemyState::Chase;
        currentFrame = 0;
        stateTimer = 0.0;
    }
}

void Enemy::SetState(EnemyState st) {
    if(st == currentState) return;
    currentState = st;
    currentFrame = 0;
    stateTimer = 0.0;

    auto& seq = frames[(int)currentState];
    if(!seq.empty()) {
        tex = seq[0].texture;
    }
}

void Enemy::Update(double deltaTime) {
    Entity::Update(deltaTime);

    auto& seq = frames[(int)currentState];
    if(seq.empty()) return;

    stateTimer += deltaTime;
    while(stateTimer >= seq[currentFrame].time) {
        stateTimer -= seq[currentFrame].time;
        currentFrame = (currentFrame + 1) % seq.size();
        tex = seq[currentFrame].texture;
    }

    //tu pozniej logika AI zmieniajaca SetState(...)
    // std::cout << this << "-" << currentFrame<< std::endl;
}

const EnemyInitiator ImpInitiator{
    "TROO",   //base_texture_name
    60,       //health
    8,        //speed
    0.5,      //reactionTime
    0.2f,     //painChance
    true,     //blocks
    EntityPosType::Floor,
    20,       //radius

    //Idle
    {
            { 'A', 10 }
    },
    //Chase (klasyczne ABCD ABCD)
    {
            { 'A', 4 }, { 'B', 4 }, { 'C', 4 }, { 'D', 4 }
    },
    //Melee
    {
            { 'E', 6 }, { 'F', 6 }, { 'G', 6 }
    },
    //Projectile (jesli ma strzelac)
    {
            { 'H', 6 }, { 'I', 6 }
    },
    //Pain
    {
            { 'J', 3 }
    },
    //Death
    {
            { 'K', 6 }, { 'L', 6 }, { 'M', 6 }, { 'N', 6 }, { 'O', 6 }
    },
    //Gib
    {
        //puste albo inne litery
    }
};
