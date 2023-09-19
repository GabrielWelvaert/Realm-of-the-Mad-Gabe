#ifndef OFFENSESTATCOMPONENT_H
#define OFFENSESTATCOMPONENT_H

#include "../Components/BaseStatComponent.h"

struct OffenseStatComponent{
    unsigned char activeattack;
    unsigned char activedexterity;

    OffenseStatComponent() 
        : activeattack(0), activedexterity(0) {}

    OffenseStatComponent(unsigned char activeattack, unsigned char activedexterity)
        : activeattack(activeattack), activedexterity(activedexterity) {}

    OffenseStatComponent(BaseStatComponent bsc)
        : activeattack(bsc.attack), activedexterity(bsc.dexterity) {}
        
};

#endif 