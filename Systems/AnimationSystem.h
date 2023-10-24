#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include "../../libs/SDL2/SDL.h"
#include "../../libs/SDL2/SDL_image.h"
#include "../ECS/ECS.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/AnimationComponent.h"
#include <algorithm>

/*
This system is reponsible for updating the sprite source rect for entities given the amount of deltatime that has passed
*/

class AnimationSystem: public System {

    public:
        AnimationSystem();

        void Update(SDL_Rect camera);
};

#endif