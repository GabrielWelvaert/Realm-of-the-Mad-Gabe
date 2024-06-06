#ifndef STANDSHOOTMINIONCOMPONENT_H
#define STANDSHOOTMINIONCOMPONENT_H

struct StandShootMinionComponent{
    unsigned short shootRange;
    float distance;

    StandShootMinionComponent() = default;

    StandShootMinionComponent(int shootRange, float distance): 
        shootRange(static_cast<unsigned short>(shootRange)),
        distance(distance)
        {}
};


#endif