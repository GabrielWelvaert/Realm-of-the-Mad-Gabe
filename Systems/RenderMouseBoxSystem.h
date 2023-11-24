#ifndef RENDERMOUSEBOXSYSTEM_H
#define RENDERMOUSEBOXSYSTEM_H

#include "../../libs/SDL2/SDL.h"
#include "../../libs/SDL2/SDL_image.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/MouseBoxComponent.h"

/*
this system is used for debugging. it renders the collision boxes which surround inventory items and loot bag items
*/

class RenderMouseBoxSystem: public System {
    public:
        RenderMouseBoxSystem();

        void Update(SDL_Renderer* renderer, SDL_Rect camera);
};

#endif