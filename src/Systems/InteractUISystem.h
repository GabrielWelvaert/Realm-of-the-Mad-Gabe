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
        std::vector<glm::vec2> bagSlotPositions = {{765, 631}, {822, 631}, {878, 631}, {934, 631}, {765, 688}, {822, 688}, {878, 688}, {934, 688}};

    public:
        InteractUISystem(){
            RequireComponent<InteractUIComponent>();
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
            eventBus->SubscribeToEvent<LootBagCollisionEvent>(this, &InteractUISystem::displayBag);
        }

        void displayBag(LootBagCollisionEvent& event){
            auto& lbc = event.lootbag.GetComponent<LootBagComponent>();
            lbc.opened = event.status;
            GetSystemEntities()[0].GetComponent<SpriteComponent>().zIndex = event.zIndex;
            int i = 0;
            while(i < 8){ // 8 item slots (1-8) (note these dont "belong" to loot bag)
                if(lbc.contents.find(i+1) != lbc.contents.end()){ // if there is an item in this inventory slot (1-8)
                    auto& item = lbc.contents[i+1];
                    const auto& position = bagSlotPositions[i];
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
            // std::string s;
            // event.status ? s = "open" : s = "close";
            // std::cout << s << " onDisplayBag event triggered; ";
            
            if(event.status){
                event.playerIC.IdOfOpenBag = event.lootbag.GetId();
                event.playerIC.viewingBag = true;
                lbc.opened = true;
            } else {
                lbc.opened = event.playerIC.viewingBag = false;
            }
            
            // if(event.status){
            //     std::cout << " opening bag " << event.lootbag.GetId() << std::endl;
            // } else {
            //     std::cout << " closing bag " << event.lootbag.GetId() << std::endl;
            // }
        }

};