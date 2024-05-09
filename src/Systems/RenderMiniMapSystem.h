#ifndef RENDERMINIMAPSYSTEM_H
#define RENDERMINIMAPSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/DistanceToPlayerComponent.h"
#include "../../libs/SDL2/SDL.h"
#include "../Components/SpriteComponent.h"
#include "../AssetStore/AssetStore.h"

class RenderMiniMapSystem: public System{
    private:
        inline bool isInMiniMapBounds(const glm::ivec2& renderSpot){
            return renderSpot.x > 755 && renderSpot.y > 5 && renderSpot.y < 237 && renderSpot.x < 987; 
        }

    public:
        RenderMiniMapSystem();
        void Update(SDL_Renderer* renderer, Entity player, int idOfMiniMap, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, int idOfboss = -1, int creationIdOfBoss = -1);

};

#endif