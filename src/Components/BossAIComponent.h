#ifndef BOSSAICOMPONENT_H
#define BOSSAICOMPONENT_H

#include "../Utils/enums.h"
#include "../Utils/tables.h"
#include "../../libs/SDL2/SDL.h"
#include "../../libs/glm/glm.hpp"

struct BossAIComponent{
    sprites bossType; // sprite enum representing what boss this is
    glm::vec2 spawnPoint; // boss will wander and return to this point; its not necessarily the actual spawn point
    Uint32 timeAwokenMS; // time that boss was activated by player
    int secondPhase; // health where boss may enter second phase, or do something else
    int survival; // health where boss may enter survival phase, or do something else
    int detectRange; // boss actives if player is this many pixels away'
    bool activated = false; 
    std::bitset<8> flags;
    Uint32 timer0 = 0; // timer that can be used for whatever
    Uint32 timer1 = 0;
    Uint32 timer2 = 0;
    Uint32 timer3 = 0;
    glm::vec2 positionflag;

    // optional. used to give boss static movement patterns
    std::vector<glm::vec2> phaseOnePositions;
    int phaseOneIndex = 0;
    std::vector<glm::vec2> phaseTwoPositions;
    int phaseTwoIndex = 0;
    std::vector<glm::vec2> phaseThreePositions;
    int phaseThreeIndex = 0;

    int idKeyOne, idKeyTwo, cIdKeyOne, cIdKeyTwo;

    inline BossAIComponent() = default;

    inline BossAIComponent(sprites bossType, glm::vec2 spawnPoint, int secondPhase, int survival, int detectRange): 
        bossType(bossType), spawnPoint(spawnPoint), timeAwokenMS(0), secondPhase(secondPhase), survival(survival), detectRange(detectRange), activated(false) {
            switch(bossType){
                case BOSSCHICKEN:{
                    for(int i = 0; i < 36; ++i) { // phase 1 destinations to make boss move in circle
                        float angle = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(36);
                        float x = spawnPoint.x + 150 * std::cos(angle);
                        float y = spawnPoint.y + 150 * std::sin(angle);
                        phaseOnePositions.push_back({x,y});
                    }
                    // phase 2 it moves between 4 corners of an imagined square
                    phaseTwoPositions.push_back({spawnPoint.x + 3 * 64, spawnPoint.y - 3 * 64});
                    phaseTwoPositions.push_back({spawnPoint.x - 3 * 64, spawnPoint.y - 3 * 64});
                    phaseTwoPositions.push_back({spawnPoint.x - 3 * 64, spawnPoint.y + 3 * 64});
                    phaseTwoPositions.push_back({spawnPoint.x + 3 * 64, spawnPoint.y + 3 * 64});
                    phaseTwoPositions.push_back({spawnPoint.x, spawnPoint.y});

                    // phase 3 it chases player
                } break;
                case ARCMAGE:{
                    // one vector should hold movement positions 
                    // one vector should hold spawn locations for fire wall
                    for(int i = 0; i < 12; i++) { // phaseOnePositions holds aiming positions for secondary projectile
                        float angle = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(12);
                        float x = spawnPoint.x + 150 * std::cos(angle);
                        float y = spawnPoint.y + 150 * std::sin(angle);
                        phaseOnePositions.push_back({x,y});
                    }

                    phaseTwoPositions.push_back({spawnPoint.x - 640, spawnPoint.y});
                    phaseTwoPositions.push_back({spawnPoint.x, spawnPoint.y});
                    phaseTwoPositions.push_back({spawnPoint.x + 580, spawnPoint.y});
                    phaseTwoPositions.push_back({spawnPoint.x, spawnPoint.y});

                } break;
                case GRANDSPHINX:
                case CRYSTALPRISONER:
                case GORDON:{
                    for(int i = 0; i < 36; i++) { // inner key pathing destinations 
                        float angle = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(36);
                        float x = (spawnPoint.x + 32) + 150 * std::cos(angle);
                        float y = (spawnPoint.y + 32) + 150 * std::sin(angle);
                        phaseOnePositions.push_back({x,y});
                    }

                    for(int i = 0; i < 36; i++) { // outer key pathing destinations
                        float angle = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(36);
                        float x = (spawnPoint.x + 32) + 350 * std::cos(angle);
                        float y = (spawnPoint.y + 32) + 350 * std::sin(angle);
                        phaseTwoPositions.push_back({x,y});
                    }
                } break;
            } 
        }


};

#endif