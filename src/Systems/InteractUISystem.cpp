#include "InteractUISystem.h"

/*
This system is responsible for managing the interact-gui (lowest portion)
ex: standing on loot bag, standing on portals
*/

// memory map of InteractUISystem
// [0] = lootbag/vault slots 
// [1] = portal entrance entity 

InteractUISystem::InteractUISystem(){
    RequireComponent<InteractUIComponent>();
}

void InteractUISystem::sortEntities(){
    auto& entities = GetSystemEntities();
    std::sort(entities.begin(), entities.end(), [](const Entity& entity1, const Entity& entity2) {return entity1.GetComponent<InteractUIComponent>().InteractUIId < entity2.GetComponent<InteractUIComponent>().InteractUIId;});
}

void InteractUISystem::SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
    eventBus->SubscribeToEvent<LootBagCollisionEvent>(this, &InteractUISystem::displayBag);
    eventBus->SubscribeToEvent<PortalCollisionEvent>(this, &InteractUISystem::displayPortal);
}

/* if status is false, event.portal is NOT guaranteed to be a portal; could be some other entity.
   This allows for portal UI to be closed when colliding w/ loot bag
*/
void InteractUISystem::displayPortal(PortalCollisionEvent& event){
    Entity& portalUI = GetSystemEntities()[1];
    auto& UIsprite = portalUI.GetComponent<SpriteComponent>();
    auto& playerIC = event.player.GetComponent<PlayerItemsComponent>();
    if(event.status){ // display portal UI
        auto& portalComponent = event.portal.GetComponent<PortalComponent>();
        UIsprite.assetId = wallThemeToPortalUITexture.at(portalComponent.area);    
        UIsprite.zIndex = 11;
        playerIC.idOfViewedPortal = event.portal.GetId();
        playerIC.viewingPortal = portalComponent.isBeingViewed = true;
        playerIC.areaOfViewedPortal = portalComponent.area;
    } else { // hide portal UI
        UIsprite.zIndex = 9;
        event.registry->GetComponent<PortalComponent>(playerIC.idOfViewedPortal).isBeingViewed = false;
        playerIC.viewingPortal = false;
    }
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
    
    if(event.status){ // opening
        playerIC.IdOfOpenBag = event.lootbag.GetId();
        playerIC.viewingBag = true;
        lbc.opened = true;
    } else { // closing
        lbc.opened = playerIC.viewingBag = false;
        // if player was displaying icon of an item residing in loot bag
        if(!event.registry->HasComponent<TransformComponent>(playerIC.hoveredItemId)){ // item hovered is from lootbag no longer viewed
            if(playerIC.displayingIcon){event.eventBus->EmitEvent<KillItemIconEvent>();}
            playerIC.hoveringItemLastFrame = playerIC.iconEntityId = playerIC.hoveredItemId = playerIC.displayingIcon = false;
            playerIC.hoverStartTime = 0 - 1;
        }
    }

}