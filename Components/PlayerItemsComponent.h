#ifndef PLAYERITEMSCOMPONENT_H
#define PLAYERITEMSCOMPONENT_H

// #include "../ECS/ECS.h"
#include "../../libs/SDL2/SDL.h"
#include <map>  
#include "../Components/ItemComponent.h"

struct PlayerItemsComponent{
    std::map<unsigned char, Entity> equipment;
    std::map<unsigned char, Entity> inventory;
    int IdOfOpenBag;
    bool viewingBag;
    bool holdingItemLastFrame = false;
    int IdOfHeldItem = 0;
    glm::vec2 heldItemStartingTransformComp;

    //variables for displaying item icon
    Uint32 hoverStartTime;
    bool hoveringItemLastFrame = false;
    bool displayingIcon = false;
    int hoveredItemId = 0;
    int iconEntityId = 0;

    inline PlayerItemsComponent() = default;

    inline void KillPlayerItems(){
        for(auto& x: equipment){
            x.second.Kill();
        }
        equipment.clear();
        for(auto& x: inventory){
            x.second.Kill();
        }
        inventory.clear();
    }
};

#endif