#include "RenderSystem.h"

RenderSystem::RenderSystem(){
    RequireComponent<TransformComponent>();
    RequireComponent<SpriteComponent>();
}

// assumes player is indeed the player and has HPMPComponent and StatusEffectComponent
void RenderSystem::RenderVeils(SDL_Renderer* renderer, Entity player, std::unique_ptr<AssetStore>& assetStore){
    SDL_Rect screen = {0,0,750,750};
    
    // rendering red veil if player hp is low below lowHealth
    auto& HPMP = player.GetComponent<HPMPComponent>();
    auto percentHealth = HPMP.activehp / HPMP.maxhp;
    constexpr int maxalpha = 128;
    constexpr float lowHealth = 0.15f;
    if(percentHealth < lowHealth){
        auto alpha = static_cast<int>(maxalpha-(maxalpha * (1 - exp(-5 * percentHealth)))); // alpha value decreases as health decreases
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 150,0,0,alpha);
        SDL_RenderFillRect(renderer, &screen);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    // rendering grey veil if the player has been blinded
    if(player.GetComponent<StatusEffectComponent>().effects[BLIND]){
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 40,40,40,150);
        SDL_RenderFillRect(renderer, &screen);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    // equipment icon veil to indicate cooldown or insufficient mp
    const auto& ac = player.GetComponent<AbilityComponent>();
    const auto& ic = player.GetComponent<PlayerItemsComponent>();
    if(!ac.abilityEquipped || (ic.holdingItemLastFrame && ic.equipment.at(2).GetId() == ic.IdOfHeldItem)){
        return; // no ability or holding ability = no need to render veil
    }
    constexpr int x = 827 - 2;
    constexpr int y = 455 - 2;
    constexpr int dimension = 50;
    auto time = SDL_GetTicks();
    const SDL_Rect veilMaskSrc = {9*44,44*2,44,44}; // this is used to cover the borders of the square veil so its clean with the curved edges of the equipment inventory slot
    const SDL_Rect veilMaskDst = {827-6,455-6,static_cast<int>(44*1.25),static_cast<int>(44*1.25)}; // strange values ensure nearly pixel-perfect appearance
    if(HPMP.activemp >= ac.mpRequired){ // display shrinking cooldown veil
        if(ac.timeLastUsed != 0 && time < ac.timeLastUsed + ac.coolDownMS){ // cooldown period has not completed
            float percentCoolDownCompleted = std::max(((time - ac.timeLastUsed) / static_cast<float>(ac.coolDownMS)), .01f);
            int height = dimension - (dimension * percentCoolDownCompleted); // height of veil depends on how much coolDown is left
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 40,40,40,150); // semi-transparent grey 
            SDL_Rect rect = {x, static_cast<int>(y + dimension - height), dimension -1, static_cast<int>(height)};    
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            SDL_RenderCopyEx(renderer,assetStore->GetTexture(INVENTORYICONS),&veilMaskSrc,&veilMaskDst,0.0,NULL,SDL_FLIP_NONE); // cover veil corners to match iventory slot's curved edges
        }
    } else { // even if not in cooldown, insufficient mana-- render full grey veil
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 40,40,40,150); // semi-transparent grey 
        SDL_Rect rect = {x, y, dimension-1, dimension};  // fully filled in veil for insufficient mana
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        SDL_RenderCopyEx(renderer,assetStore->GetTexture(INVENTORYICONS),&veilMaskSrc,&veilMaskDst,0.0,NULL,SDL_FLIP_NONE); // cover veil corners to match iventory slot's curved edges
    }

}

void RenderSystem::RenderHealthBars(SDL_Renderer* renderer, const SDL_Rect& camera, std::unique_ptr<Registry>& registry, Entity player, std::vector<BossIds>& bosses){
    for(const auto& boss: bosses){
        auto idOfboss = boss.Id;
        auto creationIdOfBoss = boss.CreationId; 
        if(registry->GetCreationIdFromEntityId(idOfboss) == creationIdOfBoss){
            const auto& sprite = registry->GetComponent<SpriteComponent>(idOfboss);
            const auto& transform = registry->GetComponent<TransformComponent>(idOfboss);
            if(transform.position.x - camera.x < 750){ // if some part of hp bar not under HUD
                auto rightcorner = transform.position.x + sprite.width * transform.scale.x - camera.x;
                int widthSubtraction = 0;
                if(rightcorner > 750){
                    widthSubtraction = rightcorner - 750;
                }
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_Rect rect = {
                    static_cast<int>(transform.position.x - camera.x), 
                    static_cast<int>(transform.position.y - camera.y + sprite.height * transform.scale.y + 4), 
                    static_cast<int>((sprite.width * transform.scale.x) - widthSubtraction),
                    static_cast<int>(8)
                };
                SDL_RenderFillRect(renderer, &rect);

                const auto& hpmp = registry->GetComponent<HPMPComponent>(idOfboss);
                auto percentHealth = hpmp.activehp / hpmp.maxhp;
                rightcorner = transform.position.x - camera.x + (sprite.width * transform.scale.x * percentHealth);
                if(rightcorner > 750){
                    widthSubtraction = rightcorner - 750;
                } else {
                    widthSubtraction = 0;
                }
                rect = {
                    static_cast<int>(transform.position.x - camera.x), 
                    static_cast<int>(transform.position.y - camera.y + sprite.height * transform.scale.y + 4), 
                    static_cast<int>((sprite.width * transform.scale.x * percentHealth) - widthSubtraction),
                    static_cast<int>(8)
                };
                SDL_SetRenderDrawColor(renderer, hpbarcolor.r, hpbarcolor.g, hpbarcolor.b, 255);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    //render player health bar 
    const auto& transform = player.GetComponent<TransformComponent>();
    const auto& sprite = player.GetComponent<SpriteComponent>();
    const auto& hpmp = player.GetComponent<HPMPComponent>();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect rect = {
        static_cast<int>(transform.position.x - camera.x), 
        static_cast<int>(transform.position.y - camera.y + sprite.height * transform.scale.y + 4), 
        static_cast<int>(sprite.width * transform.scale.x),
        static_cast<int>(8)
    };
    SDL_RenderFillRect(renderer, &rect);

    auto percentHealth = hpmp.activehp / hpmp.maxhp;
    rect = {
        static_cast<int>(transform.position.x - camera.x), 
        static_cast<int>(transform.position.y - camera.y + sprite.height * transform.scale.y + 4), 
        static_cast<int>(sprite.width * transform.scale.x * percentHealth),
        static_cast<int>(8)
    };
    SDL_SetRenderDrawColor(renderer, hpbarcolor.r, hpbarcolor.g, hpbarcolor.b, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void RenderSystem::Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, const SDL_Rect& camera, std::unique_ptr<Registry>& registry){

    auto& entities = GetSystemEntities();

    std::sort(entities.begin(), entities.end(), [](const Entity& e1, const Entity& e2){ // sorting by Z-axis and Y-position provides illusion of 3D-depth
        const auto& e1sprite = e1.GetComponent<SpriteComponent>();
        const auto& e2sprite = e2.GetComponent<SpriteComponent>(); 
        
        if(e1sprite.zIndex != e2sprite.zIndex){
            return e1sprite.zIndex < e2sprite.zIndex;
        }

        const auto& e1trans = e1.GetComponent<TransformComponent>();
        const auto& e2trans = e2.GetComponent<TransformComponent>();

        return e1trans.position.y + e1sprite.height * e1trans.scale.y < e2trans.position.y + e2sprite.height * e2trans.scale.y;

    });

    for (auto& entity: entities){
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
    }

}
