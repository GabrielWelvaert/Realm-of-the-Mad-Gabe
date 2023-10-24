#ifndef RENDERCOLLIDERSYSTEM_H
#define RENDERCOLLIDERSYSTEM_H

// this class is used for debugging. it renders box colliders

#include "../../libs/SDL2/SDL.h"
#include "../../libs/SDL2/SDL_image.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/BoxColliderComponent.h"

class RenderColliderSystem: public System {
    public:
        RenderColliderSystem();

        void Update(SDL_Renderer* renderer, SDL_Rect camera);
};

#endif