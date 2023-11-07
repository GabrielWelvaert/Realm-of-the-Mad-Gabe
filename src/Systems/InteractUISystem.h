#ifndef INTERACTUISYSTEM_H
#define INTERACTUISYSTEM_H

#include "../ECS/ECS.h"
#include "../../libs/SDL2/SDL.h"
#include <vector>
#include "../Utils/tables.h"
#include "../Utils/enums.h"
#include "../Components/LootBagComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/InteractUIComponent.h"
#include "../Components/PortalComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/LootBagCollisionEvent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/MouseBoxComponent.h"
#include "../Events/KillItemIconEvent.h"
#include "../Events/PortalCollisionEvent.h"

/*
This system is responsible for managing the interact-gui (lowest portion)
ex: standing on loot bag, standing on portals
*/

class InteractUISystem: public System{
    private:
        std::vector<glm::vec2> bagSlotPositions = {{765, 631}, {822, 631}, {878, 631}, {934, 631}, {765, 688}, {822, 688}, {878, 688}, {934, 688}};

    public:
        InteractUISystem();
        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus);
        void displayBag(LootBagCollisionEvent& event);
        void displayPortal(PortalCollisionEvent& event);
        void sort();
        // display portal button 

};

#endif