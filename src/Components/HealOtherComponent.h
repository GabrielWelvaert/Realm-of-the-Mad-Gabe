#ifndef HEALOTHERCOMPONENT_H
#define HEALOTHERCOMPONENT_H

#include "../../libs/SDL2/SDL.h"

struct HealOtherComponent{
    int beneficiaryId;
	unsigned int beneficiaryCreationId;
	bool beneficiaryIsDead = false; 
	Uint32 interval;
	Uint32 timeOfLastHeal = 0; 
    int amount;

    HealOtherComponent() = default;

    HealOtherComponent(int beneficiaryId, unsigned int beneficiaryCreationId, Uint32 interval, int amount):
        beneficiaryId(beneficiaryId), 
        beneficiaryCreationId(beneficiaryCreationId),
        interval(interval),
        amount(amount) 
        {}

};

#endif