#include "RenderMouseBoxSystem.h"

RenderMouseBoxSystem::RenderMouseBoxSystem(){
    RequireComponent<TransformComponent>();
    RequireComponent<MouseBoxComponent>();
}

void RenderMouseBoxSystem::Update(SDL_Renderer* renderer, SDL_Rect camera){
    for(auto& entity: GetSystemEntities()){
        const auto& transform = entity.GetComponent<TransformComponent>();
        const auto& collider = entity.GetComponent<MouseBoxComponent>();

        SDL_Rect colliderRect = {
            static_cast<int>(transform.position.x + collider.offset.x),
            static_cast<int>(transform.position.y + collider.offset.y),
            static_cast<int>(collider.width),
            static_cast<int>(collider.height)
        };

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &colliderRect);
    } 
}  