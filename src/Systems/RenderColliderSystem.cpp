#include "RenderColliderSystem.h"

RenderColliderSystem::RenderColliderSystem(){
    RequireComponent<TransformComponent>();
    RequireComponent<BoxColliderComponent>();
}

void RenderColliderSystem::Update(SDL_Renderer* renderer, SDL_Rect camera){
    for(auto& entity: GetSystemEntities()){
        const auto& transform = entity.GetComponent<TransformComponent>();
        const auto& collider = entity.GetComponent<BoxColliderComponent>();

        SDL_Rect colliderRect = {
            static_cast<int>(transform.position.x + collider.offset.x - camera.x),
            static_cast<int>(transform.position.y + collider.offset.y - camera.y),
            static_cast<int>(collider.width),
            static_cast<int>(collider.height)
        };

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &colliderRect);

    }
}