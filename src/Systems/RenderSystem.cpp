#include "RenderSystem.h"

RenderSystem::RenderSystem() {
    RequireComponent<TransformComponent>();
    RequireComponent<SpriteComponent>();
}

void RenderSystem::Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, const SDL_Rect& camera, std::unique_ptr<Registry>& registry, Entity player, bool renderMiniMap, int idOFMiniMap){

    //sort(begin, end, lambda function that compares two)
    auto& entities = GetSystemEntities();
    std::vector<glm::vec2> miniMapMonsters;
    miniMapMonsters.reserve(20);

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

        // store position of monsters which are fine to be rendered to mini-map
        if(entity.BelongsToGroup(MONSTER)){
            miniMapMonsters.push_back(transform.position);
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

    // icons for monsters and the player are rendered here 
    if(renderMiniMap){
        glm::ivec2 MiniMapHudSpot = {755,5};
        const int entitySquare = 8; // dimension of the icon (its just a colored square)
        const int originOfRealMap = 60; // coordinate where map actually starts (inside of padded area of texture)
        auto& srcRect = registry->GetComponent<SpriteComponent>(idOFMiniMap).srcRect;
        const auto& playerWorldPos = player.GetComponent<TransformComponent>().position;
        const int mapPixels = 240 * 64;
        const float scale = 240.0 / srcRect.w;
        glm::vec2 mmpPlayer, mmpMonster; //mini map position
        glm::ivec2 renderSpotPlayer, renderSpotMonster; // literal local coordinate to render to!
        SDL_Rect dstRectPlayer, dstRectMonster;

        // calculating player stuff for mini-map
        mmpPlayer = {(playerWorldPos.x / mapPixels) * 240.0, (playerWorldPos.y / mapPixels)* 240.0}; // real position in 240x240 mini map
        if(scale > 1.0){
            srcRect.x = mmpPlayer.x + originOfRealMap - (240 / scale / 2); // add 60 to effectively make it real position in the 360x360 texture
            srcRect.y = mmpPlayer.y + originOfRealMap - (240 / scale / 2);
            renderSpotPlayer = {MiniMapHudSpot.x + 120 - (entitySquare/4), MiniMapHudSpot.y + 120 - (entitySquare/4)}; // center of mini-map display
        } else {
            renderSpotPlayer = {MiniMapHudSpot.x + mmpPlayer.x - (entitySquare/4), MiniMapHudSpot.y + mmpPlayer.y - (entitySquare/4)};
            srcRect.x = srcRect.y = originOfRealMap; // does not follow player square
        }
        dstRectPlayer = {renderSpotPlayer.x, renderSpotPlayer.y, entitySquare, entitySquare};

        // calculating monster stuff for mini-map
        for(const auto& monsterPos: miniMapMonsters){
            mmpMonster = {(monsterPos.x / mapPixels) * 240.0, (monsterPos.y / mapPixels) * 240.0}; // real position in 240x240 mini map
            if(scale > 1.0){
                renderSpotMonster = {renderSpotPlayer.x + ((mmpMonster.x - mmpPlayer.x) * (scale)), renderSpotPlayer.y + ((mmpMonster.y - mmpPlayer.y) * (scale))};              
            } else {
                renderSpotMonster = {MiniMapHudSpot.x + mmpMonster.x - (entitySquare/4), MiniMapHudSpot.y + mmpMonster.y - (entitySquare/4)};
            }
            dstRectMonster = {renderSpotMonster.x, renderSpotMonster.y, entitySquare, entitySquare};
            SDL_SetRenderDrawColor(renderer,211,30,18,255);
            SDL_RenderFillRect(renderer, &dstRectMonster);   
        }

        //render player square last so its always visible
        SDL_SetRenderDrawColor(renderer,4,4,252,255);
        SDL_RenderFillRect(renderer, &dstRectPlayer);
        
        // uncover map veil and render it
        // void* pixels;
        // int pitch;
        // SDL_Rect miniMapRect = {755,5,250,250};
        // SDL_Texture * veilTexture = assetStore->GetTexture(MINIMAPVEIL);
        // SDL_LockTexture(veilTexture, NULL, &pixels, &pitch);
        // Uint32* pixelData = (Uint32*)pixels;
        // for(int y = (playerWorldPos.y/64) - 5; y <= (playerWorldPos.y/64) + 5; ++y) {
        //     for(int x = (playerWorldPos.x/64) - 5; x <= (playerWorldPos.x/64) + 5; ++x) {
        //         if(x >= 0 && x < 360 && y >= 0 && y < 360) {
        //             pixelData[y * 360 + x] &= 0x00FFFFFF; // Set alpha to 0 via bitwise AND
        //         }
        //     }
        // }
        // SDL_UnlockTexture(veilTexture);
        // SDL_RenderCopy(renderer, veilTexture, NULL, &miniMapRect);
        // SDL_RenderPresent(renderer);
    }

}
