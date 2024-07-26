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
    glm::vec2 velocityflag;
    aistates state = STANDING;
    int phaseflag = -1;
    int healthflag = -1;
    int timingflag = 0;

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
                case GORDON2:{
                    for(int i = 0; i < 36; i++) { // shooting angles for when at room center 
                        float angle = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(36);
                        float x = (spawnPoint.x + 32) + 600 * std::cos(angle);
                        float y = (spawnPoint.y + 32) + 600 * std::sin(angle);
                        phaseOnePositions.push_back({x,y});
                    }
                    // origin of wallshots!
                    phaseTwoPositions = {{1298, 2384}, {1553, 2384}, {2450, 1537}, {2450, 1223}, {1595, 410}, {1305, 410}, {463, 1243}, {463, 1586}, {1447.08, 2379}, {2446.65, 1382.42} ,{1448.28, 401.289}, {458.051, 1387.04}, {447, 1095}, {508, 836}, {575, 712}, {636, 636}, {699, 571}, {766, 507}, {891, 451}, {1150, 379}, {1743, 379}, {2004, 448}, {2129, 509}, {2192, 581}, {2259, 644}, {2320, 710}, {2382, 839}, {2450, 1099}, {2447, 1744}, {2369, 1808}, {2315, 2000}, {2259, 2128}, {2181, 2188}, {2128, 2253}, {1990, 2320}, {1744, 2384}, {1161, 2381}, {896, 2316}, {773, 2256}, {707, 2192}, {647, 2128}, {575, 1994}, {527, 1804}, {459, 1740}};

                    // for the 4 sentiels. assuming they will be 16x16 sprites 
                    phaseThreePositions = {{446, 1392.56}, {2380.17, 1392.56}, {1415.61, 391.366}, {1418.34, 2311.43}};
                } break;
            } 
        }


};

#endif