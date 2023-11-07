#include "PortalSystem.h"

/* This system hides portal UI for portals which were previously being viewed but are not anymore*/

PortalSystem::PortalSystem(){
    RequireComponent<PortalComponent>();
    RequireComponent<BoxColliderComponent>();
    RequireComponent<TransformComponent>();
}

void PortalSystem::Update(Entity& player, std::unique_ptr<EventBus>& eventBus, std::unique_ptr<Registry>& registry){
    const auto& playerIC = player.GetComponent<PlayerItemsComponent>();
    if(!playerIC.viewingPortal){return;}
    for(auto& entity: GetSystemEntities()){
        auto& portalComponent = entity.GetComponent<PortalComponent>();
        if(portalComponent.isBeingViewed){
            const auto& aCollider = entity.GetComponent<BoxColliderComponent>();
            const auto& aTransform = entity.GetComponent<TransformComponent>();
            const auto& bCollider = player.GetComponent<BoxColliderComponent>();
            const auto& bTransform = player.GetComponent<TransformComponent>();
            if(!CheckAABBCollision(aTransform.position.x + aCollider.offset[0],aTransform.position.y + aCollider.offset[1],aCollider.width,aCollider.height,bTransform.position.x + bCollider.offset[0],bTransform.position.y + bCollider.offset[1],bCollider.width,bCollider.height)){ 
                eventBus->EmitEvent<PortalCollisionEvent>(false, player, entity, registry);
            }
        }
    }
}