#ifndef ANIMATEDPOUNCEAICOMPONENT_H
#define ANIMATEDPOUNCEAICOMPONENT_H

#include "../Utils/enums.h"
#include "../Utils/tables.h"
#include "../../libs/SDL2/SDL.h"
#include "../Utils/Xoshiro256.h"

struct AnimatedPounceAIComponent{
    unsigned short detectRange; // distance in pixels where monster can detect player (ex: path to player)
    unsigned short engageRange; // distance where monster will further engage if possible (ex: shoot)
    unsigned short shootRange; 
    Uint32 lastPounceTime = 0;
    Uint32 lastShootTime = 0;
    std::vector<int> speeds; // [0] = walk, run, pounce speeds
    int pounceCooldown;
    bool pouncing = false;
    bool justpounced = false;

    inline AnimatedPounceAIComponent() = default;

    inline AnimatedPounceAIComponent(sprites se, Xoshiro256& RNG){
        const auto& data = spritesToPounceData.at(se);
        const auto& pec = spriteEnumToPEC.at(se);
        this->detectRange = data.detectRange;
        this->speeds = data.speeds;
        this->engageRange = data.engageRange;
        this->pounceCooldown = data.cooldown;
        this->shootRange = data.shootRange;
        data.detectRange < engageRange ? this->detectRange = engageRange : this->detectRange = data.detectRange;
    }

};

#endif 