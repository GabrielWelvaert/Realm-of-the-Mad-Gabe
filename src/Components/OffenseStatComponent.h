#ifndef OFFENSESTATCOMPONENT_H
#define OFFENSESTATCOMPONENT_H

#include "../Components/BaseStatComponent.h"

struct OffenseStatComponent{
    unsigned char activeattack;
    unsigned char activedexterity;

    inline OffenseStatComponent() 
        : activeattack(0), activedexterity(0) {}

    inline OffenseStatComponent(unsigned char activeattack, unsigned char activedexterity)
        : activeattack(activeattack), activedexterity(activedexterity) {}

    inline OffenseStatComponent(BaseStatComponent bsc)
        : activeattack(bsc.attack), activedexterity(bsc.dexterity) {}
        
};

#endif 