#include "RenderMiniMapSystem.h"

#define MAX_DINSTANCE_FOR_MINI_MAP_RENDERING 1250.0

RenderMiniMapSystem::RenderMiniMapSystem(){
    RequireComponent<TransformComponent>();
    RequireComponent<DistanceToPlayerComponent>();
}

void RenderMiniMapSystem::Update(SDL_Renderer* renderer, Entity player, int idOfMiniMap, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, std::vector<BossIds>& bosses){
    glm::ivec2 MiniMapHudSpot = {755,5};
    const int entitySquare = 8; // dimension of the icon (its just a colored square)
    const int originOfRealMap = 60; // coordinate where map actually starts (inside of padded area of texture)
    auto& srcRect = registry->GetComponent<SpriteComponent>(idOfMiniMap).srcRect;
    const auto& playerWorldPos = player.GetComponent<TransformComponent>().position;
    const int mapPixels = 240 * 64;
    const float scale = 240.0 / srcRect.w;
    glm::vec2 mmpPlayer, mmpMonster; // mini map position
    glm::ivec2 renderSpotPlayer, renderSpotMonster; // literal local coordinate to render to!
    SDL_Rect dstRectPlayer, dstRectMonster; // dstRect of literal local coordinate to render to!

    // calculating player stuff for mini-map (must be done before monsters)
    mmpPlayer = {(playerWorldPos.x / mapPixels) * 240.0, (playerWorldPos.y / mapPixels) * 240.0}; // real position in 240x240 mini map
    if(scale > 1.0){
        srcRect.x = mmpPlayer.x + originOfRealMap - (240 / scale / 2); // add originOfRealMap to effectively make it real position in the 360x360 texture
        srcRect.y = mmpPlayer.y + originOfRealMap - (240 / scale / 2);
        renderSpotPlayer = {MiniMapHudSpot.x + 120 - (entitySquare/4), MiniMapHudSpot.y + 120 - (entitySquare/4)}; // center of mini-map display
    } else {
        renderSpotPlayer = {MiniMapHudSpot.x + mmpPlayer.x - (entitySquare/4), MiniMapHudSpot.y + mmpPlayer.y - (entitySquare/4)};
        srcRect.x = srcRect.y = originOfRealMap; // does not follow player square; srcRect origin is start of map (60,60)
    }
    dstRectPlayer = {renderSpotPlayer.x, renderSpotPlayer.y, entitySquare, entitySquare};

    // calculating stuff for monsters (bosses can be wastefully rendered here too it doesn't matter)
    for(auto& entity: GetSystemEntities()){
        auto& distanceToplayer = entity.GetComponent<DistanceToPlayerComponent>().distanceToPlayer;
        auto& monsterPos = entity.GetComponent<TransformComponent>().position;
        if(distanceToplayer < MAX_DINSTANCE_FOR_MINI_MAP_RENDERING){
            mmpMonster = {(monsterPos.x / mapPixels) * 240.0, (monsterPos.y / mapPixels) * 240.0}; // real position in 240x240 mini map
            if(scale > 1.0){
                renderSpotMonster = {renderSpotPlayer.x + ((mmpMonster.x - mmpPlayer.x) * (scale)), renderSpotPlayer.y + ((mmpMonster.y - mmpPlayer.y) * (scale))};              
            } else {
                renderSpotMonster = {MiniMapHudSpot.x + mmpMonster.x - (entitySquare/4), MiniMapHudSpot.y + mmpMonster.y - (entitySquare/4)};
            }
            if(!isInMiniMapBounds(renderSpotMonster)){ // dont render if its not in the mini map
                continue;
            }
            dstRectMonster = {renderSpotMonster.x, renderSpotMonster.y, entitySquare, entitySquare};
            SDL_SetRenderDrawColor(renderer,211,30,18,255);
            SDL_RenderFillRect(renderer, &dstRectMonster); 
        }
    }

    // uncover portion of mini-map veil 
    SDL_Texture * veilTexture = assetStore->GetTexture(MINIMAPVEIL);
    void* pixels;
    int pitch;
    glm::ivec2 ptp = {static_cast<int>(mmpPlayer.x + originOfRealMap), static_cast<int>(mmpPlayer.y + originOfRealMap)}; //player texture position
    SDL_LockTexture(veilTexture, NULL, &pixels, &pitch);
    Uint32* pixelData = (Uint32*)pixels;
    int pitch4 = pitch / 4;
    int pixelsToModify = 10;
    for(int y = ptp.y - pixelsToModify; y < ptp.y + pixelsToModify; y++){
        for(int x = ptp.x - pixelsToModify; x < ptp.x + pixelsToModify; x++){
            pixelData[y * pitch4 + x] = 0x00000000; // iterating over 1d array of pixels
        }
    }
    SDL_UnlockTexture(veilTexture);

    // render mini-map veil
    SDL_Rect miniMapRect = {755,5,240,240};
    SDL_RenderCopy(renderer, veilTexture, &srcRect, &miniMapRect); // veil uses srcRect of mini map

    // render boss squares (allowed to be on top of veil)
    for(const auto& boss: bosses){
        auto idOfboss = boss.Id;
        auto creationIdOfBoss = boss.CreationId; 
        if(registry->GetCreationIdFromEntityId(idOfboss) == creationIdOfBoss){ // if we're in an area that has a boss, and its still alive...
            const auto& transform = registry->GetComponent<TransformComponent>(idOfboss);
            mmpMonster = {(transform.position.x / mapPixels) * 240.0, (transform.position.y / mapPixels) * 240.0}; // real position in 240x240 mini map
            if(scale > 1.0){
                renderSpotMonster = {renderSpotPlayer.x + ((mmpMonster.x - mmpPlayer.x) * (scale)), renderSpotPlayer.y + ((mmpMonster.y - mmpPlayer.y) * (scale))};              
            } else {
                renderSpotMonster = {MiniMapHudSpot.x + mmpMonster.x - (entitySquare/4), MiniMapHudSpot.y + mmpMonster.y - (entitySquare/4)};
            }
            dstRectMonster = {renderSpotMonster.x, renderSpotMonster.y, entitySquare, entitySquare};
            if(dstRectMonster.x > 750 && dstRectMonster.x < 990 - entitySquare && dstRectMonster.y > 5 && dstRectMonster.y < 245 - entitySquare){
                SDL_SetRenderDrawColor(renderer,211,30,18,255);
                SDL_RenderFillRect(renderer, &dstRectMonster);       
            } 
        }
    }

    // render player squares (impossible to be under veil and should always be on top)
    SDL_SetRenderDrawColor(renderer,4,4,252,255);
    SDL_RenderFillRect(renderer, &dstRectPlayer);
}