#pragma once 

#include "../ECS/ECS.h"
#include "../../libs/SDL2/SDL.h"
#include <vector>
#include "../Utils/tables.h"
#include "../Utils/enums.h"
#include "../Components/LootBagComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/InteractUIComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/LootBagCollisionEvent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/MouseBoxComponent.h"

/*
This system is responsible for managing the interact-gui (lowest portion)
ex: standing on loot bag, standing on portals
*/

class InteractUISystem: public System{
    private:
        bool wasClickingLastFrame = false;

    public:
        InteractUISystem(){
            RequireComponent<InteractUIComponent>();
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
            eventBus->SubscribeToEvent<LootBagCollisionEvent>(this, &InteractUISystem::displayBag);
        }

        void displayBag(LootBagCollisionEvent& event){
            //std::cout << "displayBag event " << event.lootbag.GetId() << " with " << static_cast<int>(event.zIndex) << " Zindex..." << std::endl;
            auto& lbc = event.lootbag.GetComponent<LootBagComponent>();
            lbc.opened = event.status;
            int i = 0;
            for(auto bagSlot: GetSystemEntities()){ // 8 item slots (1-8) (note these dont "belong" to loot bag)
                bagSlot.GetComponent<SpriteComponent>().zIndex = event.zIndex; //make inventory spot visisble/invisible
                auto& lbc = event.lootbag.GetComponent<LootBagComponent>();
                if(lbc.contents.find(i+1) != lbc.contents.end()){ // if there is an item in this inventory slot (1-8)
                    auto& item = lbc.contents[i+1];
                    const auto& position = bagSlot.GetComponent<TransformComponent>().position;
                    if(event.status){ // making bag contents visible
                        event.registry->RemoveEntityFromSystems(item);
                        item.AddComponent<TransformComponent>(glm::vec2(position.x + 8.5, position.y + 8.5), glm::vec2(5.0,5.0));
                        item.AddComponent<MouseBoxComponent>(40,40);
                        event.registry->AddEntityToSystems(item);
                    } else { // makaing bag contents invisible
                        event.registry->RemoveEntityFromSystems(item);
                        item.RemoveComponent<TransformComponent>(); 
                        item.RemoveComponent<MouseBoxComponent>();
                        event.registry->AddEntityToSystems(item); 
                    }
                } 
                i++;
            }
            // updating player InventoryComponent's pointer to open bag (nullptr if not open bag)
            event.status ? event.playerIC.ptrToOpenBag = &lbc.contents : event.playerIC.ptrToOpenBag = nullptr;
        }

};