#ifndef BOOMERANGCOMPONENT_H
#define BOOMERANGCOMPONENT_H

struct BoomerangComponent{
    bool invertedVelocity = false;
    int halfduration; 
    BoomerangComponent() = default;
    BoomerangComponent(int halfduration): halfduration(halfduration) {}
};

#endif