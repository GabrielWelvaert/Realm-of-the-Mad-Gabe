#include "RenderSystem.h"

RenderSystem::RenderSystem() {
    RequireComponent<TransformComponent>();
    RequireComponent<SpriteComponent>();
}

void RenderSystem::Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, SDL_Rect camera){

    //sort(begin, end, lambda function that compares two)
    auto& entities = GetSystemEntities();

    std::sort(entities.begin(), entities.end(), [](const Entity& e1, const Entity& e2){
        const auto& e1sprite = e1.GetComponent<SpriteComponent>();
        const auto& e2sprite = e2.GetComponent<SpriteComponent>(); 
        
        if(e1sprite.zIndex != e2sprite.zIndex){
            return e1sprite.zIndex < e2sprite.zIndex;
        }

        const auto& e1trans = e1.GetComponent<TransformComponent>();
        const auto& e2trans = e2.GetComponent<TransformComponent>();

        return e1trans.position.y + e1sprite.height * e1trans.scale.y < e2trans.position.y + e2sprite.height * e2trans.scale.y;

    });

    for (auto entity: entities){
        const auto& transform = entity.GetComponent<TransformComponent>();
        const auto& sprite = entity.GetComponent<SpriteComponent>();

        // if entity if off-camera and not fixed, do not render
        if((!sprite.isFixed) &&
            (transform.position.x + sprite.width * transform.scale.x < camera.x || 
            transform.position.y + sprite.height * transform.scale.y < camera.y || 
            transform.position.x > camera.x + camera.w || 
            transform.position.y > camera.y + camera.h)){    
            continue;
        }
        
        SDL_RendererFlip flip = sprite.flip;
        SDL_Rect srcRect = sprite.srcRect;

        //destination rect
        SDL_Rect dstRect = {
            static_cast<int>(transform.position.x - camera.x * !sprite.isFixed), 
            static_cast<int>(transform.position.y - camera.y * !sprite.isFixed),
            static_cast<int>(sprite.width * transform.scale.x),
            static_cast<int>(sprite.height * transform.scale.y)
        };

        if(entity.HasComponent<AnimationComponent>()){
            const auto& animation = entity.GetComponent<AnimationComponent>();
            int extrapixels = 0;
            if(entity.HasComponent<AnimatedShootingComponent>()){
                const auto& asc = entity.GetComponent<AnimatedShootingComponent>();
                if(asc.animatedShooting && animation.currentFrame == 1){
                    extrapixels = asc.extraPixels;
                    srcRect.w += extrapixels;
                    dstRect.w += extrapixels * transform.scale.x;
                }                     
            }
            if(sprite.flip == SDL_FLIP_HORIZONTAL){
                dstRect.x -= extrapixels * transform.scale.x;
            }
        }

        SDL_RenderCopyEx( //can pass rotation 
            renderer, 
            assetStore->GetTexture(sprite.assetId),
            &srcRect,
            &dstRect,
            transform.rotation,
            NULL,
            flip
        );

        // std::cout << entity.GetId() << std::endl;

    }
    // std::cout << std::endl;
}
