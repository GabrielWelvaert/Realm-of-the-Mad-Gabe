#ifndef ORBITSHOOTMINIONCOMPONENT_H
#define ORBITSHOOTMINIONCOMPONENT_H

struct OrbitShootMinionComponent{
    unsigned short shootRange;

    OrbitShootMinionComponent() = default;

    OrbitShootMinionComponent(int shootRange): shootRange(static_cast<unsigned short>(shootRange)) {}

};

#endif