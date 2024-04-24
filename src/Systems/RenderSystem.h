#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H
#include "../../libs/SDL2/SDL.h"
#include "../../libs/SDL2/SDL_image.h"
#include "../ECS/ECS.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/AnimationComponent.h"
#include "../AssetStore/AssetStore.h"
#include <algorithm>
#include "../Components/AnimatedShootingComponent.h"
#include "../Components/HPMPComponent.h"

/*
This system is responsible for drawing sprites on the screen. It sorts them by their z-axis and y-position and takes into account animationComponents
*/

class RenderSystem: public System {
    public:
        RenderSystem();
        void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, const SDL_Rect& camera, std::unique_ptr<Registry>& registry, Entity player, bool renderMiniMap,int idOFMiniMap = 0, int idOfboss = -1, int creationIdOfBoss = -1, bool renderHealthBars = false);
};

#endif 