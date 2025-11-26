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
    for(int s = 0; s < (int)EnemyState::Count; ++s) {
        auto& loaderSeq = init.loaders[s];
        auto& outSeq = frames[s];
        outSeq.clear();

        for(const auto& fl : loaderSeq) {
            EnemyFrame fr{};
            fr.time = fl.time * ticToSeconds;

            bool anyAngleLoaded = false;
            std::string base = init.base_texture_name;
            char c = fl.let;

            for(int ai = 0; ai < 8; ++ai) {
                int doomAngle = ai + 1; //0..7 -> 1..8
                gl_texture chosen{};
                bool found = false;
                bool mirror = false;

                std::vector<std::string> candidates;

                //kandydaci zalezni od kata
                switch(doomAngle) {
                    case 1:
                        candidates.push_back(base + c + "1");
                        break;
                    case 5:
                        candidates.push_back(base + c + "5");
                        break;
                    case 2:
                        candidates.push_back(base + c + "2");    //osobny lump jesli jest
                        candidates.push_back(base + c + "2" + c + "8");  //wspolny dla 2 i 8
                        break;
                    case 8:
                        candidates.push_back(base + c + "8");
                        candidates.push_back(base + c + "2" + c + "8");
                        break;
                    case 3:
                        candidates.push_back(base + c + "3");
                        candidates.push_back(base + c + "3" + c + "7");
                        break;
                    case 7:
                        candidates.push_back(base + c + "7");
                        candidates.push_back(base + c + "3" + c + "7");
                        break;
                    case 4:
                        candidates.push_back(base + c + "4");
                        candidates.push_back(base + c + "4" + c + "6");
                        break;
                    case 6:
                        candidates.push_back(base + c + "6");
                        candidates.push_back(base + c + "4" + c + "6");
                        break;
                }

                for(const auto& name : candidates) {
                    auto texTry = tb->GetTexture(name.c_str(), TextureType::ItemTexture);
                    if(isValidTexture(texTry)) {
                        chosen = texTry;
                        found = true;

                        bool combined =
                            name.find(std::string("2") + c + "8") != std::string::npos ||
                            name.find(std::string("3") + c + "7") != std::string::npos ||
                            name.find(std::string("4") + c + "6") != std::string::npos;

                        if(combined) {
                            //druga strona pary odbijana lustrzanie
                            if(doomAngle == 8 || doomAngle == 7 || doomAngle == 6)
                                mirror = true;
                        }

                        break;
                    }
                }
                fr.angles[ai] = found ? chosen : gl_texture{};
                fr.mirror[ai] = mirror;
                anyAngleLoaded |= found;
            }

            if(anyAngleLoaded) {
                outSeq.push_back(fr);
            }
        }
    }

    //startowy stan: idle
    auto& idle = frames[(int)EnemyState::Idle];
    if(!idle.empty()) {
        gl_texture first = idle[0].angles[0];
        if(!isValidTexture(first)) {
            for(int ai = 1; ai < 8; ++ai) {
                if(isValidTexture(idle[0].angles[ai])) {
                    first = idle[0].angles[ai];
                    break;
                }
            }
        }
        if(isValidTexture(first)) {
            tex = first;
        }
        currentState = EnemyState::Idle;
        currentFrame = 0;
        stateTimer = 0.0;
    }
}

void Enemy::SetState(EnemyState st) {
    if(st == currentState) return;
    currentState = st;
    currentFrame = 0;
    stateTimer = 0.0;
}

void Enemy::Update(double deltaTime) {
    Entity::Update(deltaTime);

    auto& seq = frames[(int)currentState];
    if (seq.empty()) return;

    stateTimer += deltaTime;
    while (stateTimer >= seq[currentFrame].time) {
        stateTimer -= seq[currentFrame].time;
        currentFrame = (currentFrame + 1) % seq.size();
    }

    //Logika
    SetState(EnemyState::Chase);
}

void Enemy::Render(fvec2 playerPosition) const {
    const auto& seq = frames[(int)currentState];
    if (seq.empty()) {
        //brak animacji - rysuj jak zwykle
        Entity::Render(playerPosition);
        return;
    }

    const EnemyFrame& frame = seq[currentFrame];

    //wektor od gracza do wroga
    fvec2 toEntity = (fvec2)position - playerPosition;
    float len2 = toEntity.length_sq();
    if(len2 < 1e-6f) {
        Entity::Render(playerPosition);
        return;
    }

    float ang = atan2(-toEntity.y, toEntity.x); //cw
    if(ang < 0.f) ang += 2.f * (float)M_PI;

    float slice = 2.f * (float)M_PI / 8.f;
    int rotIndex = (int)((ang + slice * 0.5f) / slice) & 7;

    gl_texture curTex = frame.angles[rotIndex];
    bool mirror = frame.mirror[rotIndex];

    if(!isValidTexture(curTex)) {
        //fallback front / inne katy
        for(int i = 0; i < 8; ++i) {
            if(isValidTexture(frame.angles[i])) {
                curTex = frame.angles[i];
                mirror = frame.mirror[i];
                break;
            }
        }
        if(!isValidTexture(curTex)) {
            Entity::Render(playerPosition);
            return;
        }
    }


    //ponizej to jest w zasadzie skopiowany Entity::Render, tylko uzywa curTex

    fvec2 right = toEntity.perpendicular().normalized();

    float halfWidth = curTex.w / 2.f;
    fvec2 offset = right * halfWidth;

    fvec2 bottomLeft  = (fvec2)position - offset;
    fvec2 bottomRight = (fvec2)position + offset;
    fvec2 topLeft     = bottomLeft;
    fvec2 topRight    = bottomRight;

    float bottomY = start_height;
    float topY    = start_height + curTex.h;

    //u koordy zalezne od mirror
    float uLeft  = mirror ? 1.0f : 0.0f;
    float uRight = mirror ? 0.0f : 1.0f;

    glBindTexture(GL_TEXTURE_2D, curTex.texture_id);
    glBegin(GL_TRIANGLES);
    glTexCoord2f(uLeft,  1.f); glVertex3f(bottomLeft.x,  bottomY, -bottomLeft.y);
    glTexCoord2f(uRight, 1.f); glVertex3f(bottomRight.x, bottomY, -bottomRight.y);
    glTexCoord2f(uRight, 0.f); glVertex3f(topRight.x,    topY,    -topRight.y);

    glTexCoord2f(uLeft,  1.f); glVertex3f(bottomLeft.x,  bottomY, -bottomLeft.y);
    glTexCoord2f(uRight, 0.f); glVertex3f(topRight.x,    topY,    -topRight.y);
    glTexCoord2f(uLeft,  0.f); glVertex3f(topLeft.x,     topY,    -topLeft.y);
    glEnd();

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
