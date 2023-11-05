#include "InteractUISystem.h"

InteractUISystem::InteractUISystem(){
    RequireComponent<InteractUIComponent>();
}

void InteractUISystem::SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
        eventBus->SubscribeToEvent<LootBagCollisionEvent>(this, &InteractUISystem::displayBag);
    }

void InteractUISystem::displayBag(LootBagCollisionEvent& event){
    auto& lbc = event.lootbag.GetComponent<LootBagComponent>();
    auto& playerIC = event.player.GetComponent<PlayerItemsComponent>();
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
    
    if(event.status){
        playerIC.IdOfOpenBag = event.lootbag.GetId();
        playerIC.viewingBag = true;
        lbc.opened = true;
    } else {
        lbc.opened = playerIC.viewingBag = false;
        // if player was displaying icon of an item residing in loot bag
        if(playerIC.displayingIcon && !event.registry->HasComponent<TransformComponent>(playerIC.hoveredItemId)){ // if hovered item lacks transformComp, it must be from lootbag not viewed anymore
            event.eventBus->EmitEvent<KillItemIconEvent>();    
            playerIC.hoveringItemLastFrame = playerIC.iconEntityId = playerIC.hoveredItemId = playerIC.displayingIcon = false;
        }
    }

}