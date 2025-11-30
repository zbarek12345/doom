#include "headers/Enemy.h"

#include <iostream>

#include "headers/Player.h"
#include "headers/TexBinder.h"

static bool isValidTexture(const gl_texture& t) {
    return t.texture_id != 0 && t.w > 0 && t.h > 0;
}

Enemy::Enemy(svec2 position,
             const EnemyInitiator& init,
             NewModels::Map* map,
             NewModels::Sector* sector)
    : Entity(position,
             init.radius,
             init.base_texture_name,
             "",
             init.blocks,
             init.pos_type),
      map(map),
      currentSector(sector)
{
    health = init.health;
    speed = init.speed;
    reactionTime = init.reactionTime;
    painChance = init.painChance;

    //wysokosc startowa z sektora (floor)
    float z = sector ? (float)sector->floor_height : 0.f;
    pos3 = fvec3((float)position.x, z, (float)position.y);
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

void Enemy::TakeDamage(uint16_t dmg) {
    if (health == 0) return;

    if (dmg >= health) {
        health = 0;
        SetState(EnemyState::Death);
        return;
    }

    health -= dmg;

    float r = (float)rand() / (float)RAND_MAX;
    if (r < painChance) {
        SetState(EnemyState::Pain);
    }
}

void Enemy::Update(double deltaTime) {
    //bez mapy/sektora nic nie robimy
    if (!map || !currentSector) {
        Entity::Update(deltaTime);
        return;
    }

    //aktualizacja sektora i wysokosci
    currentSector = map->getPlayerSector(
        svec2{static_cast<short>(pos3.x), static_cast<short>(pos3.z)},
        currentSector
    );
    pos3.y = currentSector->floor_height;

    //odliczanie cooldownu ciosu
    meleeAttackCooldown -= deltaTime;
    if (meleeAttackCooldown < 0.0) meleeAttackCooldown = 0.0;

    //pozycja gracza i dystans 2d
    fvec3 playerPos3 = Player::GetPosition();
    fvec3 toPlayer3 = playerPos3 - pos3;
    float dist2D = fvec2{toPlayer3.x, toPlayer3.z}.length();

    //death: tylko animacja, bez ruchu
    if (health == 0) {
        SetState(EnemyState::Death);
    } else {
        //budzenie z idle
        if (!isAwake) {
            if (dist2D < wakeDistance) {
                isAwake = true;
            } else {
                SetState(EnemyState::Idle);
            }
        }

        //pain: tylko animacja, bez ruchu
        if (currentState == EnemyState::Pain) {
            //nic nie robimy, animacja nizej
        } else if (isAwake && health > 0) {
            //melee vs chase
            if (dist2D <= meleeRange) {
                SetState(EnemyState::Melee);

                //prosty melee: co meleeAttackInterval sekund zadaj dmg
                if (meleeAttackCooldown == 0.0f) {
                    Player::TakeDamage(meleeDamage);
                    meleeAttackCooldown = meleeAttackInterval;
                }
            } else {
                SetState(EnemyState::Chase);

                //ruch w strone gracza
                fvec3 prevPos = pos3;
                fvec3 move = toPlayer3;
                float len = move.length();
                if (len > 1e-3f) {
                    move /= len;
                    move *= (float)speed * (float)deltaTime;
                } else {
                    move = fvec3(0,0,0);
                }

                //ruch z kolizjami (na razie jak u ciebie)
                //map->HandleMovement(move, pos3, currentSector);
                pos3 += move;

                //delta ruchu i kierunek
                fvec3 delta = pos3 - prevPos;
                if (delta.length_sq() > 1e-6f) {
                    lastMoveDir = fvec2{delta.x, delta.z}.normalized();
                }
            }
        }
    }

    //animacja po ustaleniu currentState
    auto& seq = frames[(int)currentState];
    if (!seq.empty()) {
        stateTimer += deltaTime;
        while (stateTimer >= seq[currentFrame].time) {
            stateTimer -= seq[currentFrame].time;
            currentFrame = (currentFrame + 1) % seq.size();
        }
    }

    //zaktualizuj 2d position dla rendera/billboardu
    position.x = (int16_t)pos3.x;
    position.y = (int16_t)pos3.z;

    Entity::Update(deltaTime);
}


void Enemy::Render(fvec2 playerPosition) const {
    const auto& seq = frames[(int)currentState];
    if (seq.empty()) {
        Entity::Render(playerPosition);
        return;
    }

    const EnemyFrame& frame = seq[currentFrame];

    //wektor od gracza do wroga (do ustawienia billboardu)
    fvec2 toEntity = fvec2{pos3.x, pos3.z} - playerPosition;
    float len2 = toEntity.length_sq();
    if(len2 < 1e-6f) {
        Entity::Render(playerPosition);
        return;
    }

    //wektor od wroga do gracza (z punktu widzenia wroga)
    fvec2 toPlayer = (playerPosition - fvec2{pos3.x, pos3.z});
    //kierunek "przodu" wroga:
    //jesli sie poruszal -> ostatni kierunek ruchu
    //jesli stoi -> niech "patrzy" na gracza
    fvec2 facingDir;
    if (lastMoveDir.length_sq() > 1e-6f)
        facingDir = lastMoveDir.normalized();
    else
        facingDir = toPlayer; //jak stoi, obrocony twarza do gracza

    //kat wzgledny miedzy przodem wroga a graczem
    //0 = gracz przed wrogiem, pi = za plecami, +-pi/2 = boki
    float dot = facingDir.x * toPlayer.x + facingDir.y * toPlayer.y;
    float det = facingDir.x * toPlayer.y - facingDir.y * toPlayer.x;
    float ang = atan2(det, dot); // -pi..pi (ccw)

    //zamieniamy na kat CW w [0, 2pi)
    float angCW = -ang;
    if (angCW < 0.f) angCW += 2.f * (float)M_PI;

    float slice = 2.f * (float)M_PI / 8.f;
    int rotIndex = (int)((angCW + slice * 0.5f) / slice) & 7;

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

    //billboard dalej prostopadle do kierunku gracz->wróg
    fvec2 right = toEntity.perpendicular().normalized();

    float halfWidth = curTex.w / 2.f;
    fvec2 offset = right * halfWidth;

    fvec2 bottomLeft  = fvec2{pos3.x, pos3.z} - offset;
    fvec2 bottomRight = fvec2{pos3.x, pos3.z} + offset;
    fvec2 topLeft     = bottomLeft;
    fvec2 topRight    = bottomRight;

    float bottomY = start_height;
    float topY    = start_height + curTex.h;

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
    12,       //speed
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
