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

/*
This system is responsible for drawing sprites on the screen. It sorts them by their z-axis and y-position and takes into account animationComponents
*/

class RenderSystem: public System {
    public:
        RenderSystem();

        void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, SDL_Rect camera);
};

#endif 