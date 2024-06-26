#ifndef MINIONCOMPONENT_H
#define MINIONCOMPONENT_H

#include "../../libs/SDL2/SDL.h"
#include "../Utils/enums.h"
#include "../../libs/glm/glm.hpp"
#include "../../libs/glm/gtc/random.hpp"

struct MinionComponent{
    int idOfParent;
    unsigned int creationIdOfParent;
    glm::vec2 escapeVelocity;
    sprites sprite; // minions may want to know who they are
    MinionComponent() = default;
    MinionComponent(int idOfParent, unsigned int creationIdOfParent, sprites sprite = NONESPRITE):
        idOfParent(idOfParent), creationIdOfParent(creationIdOfParent), sprite(sprite) {
            float randomAngle = glm::linearRand(0.0f, 6.2831855f);
            escapeVelocity = {glm::cos(randomAngle), glm::sin(randomAngle)};
        }
};

#endif