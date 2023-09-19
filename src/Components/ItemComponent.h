#pragma once 

#include "../../libs/SDL2/SDL.h"
#include "../ECS/ECS.h"
#include <memory>
#include <map>

struct ItemComponent{
    unsigned char lastPosition;
    unsigned int creationIdOfParentBag; //maybe not needed
    int idOfParentBag; //maybe not needed
    std::map<unsigned char, Entity>* hostMap;

    ItemComponent() = default;

    ItemComponent(unsigned char lastPosition, int idOfParentBag, unsigned int creationIdOfParentBag, std::map<unsigned char, Entity>& hostMap)
        : lastPosition(lastPosition), creationIdOfParentBag(creationIdOfParentBag), idOfParentBag(idOfParentBag), hostMap(&hostMap) {}

};