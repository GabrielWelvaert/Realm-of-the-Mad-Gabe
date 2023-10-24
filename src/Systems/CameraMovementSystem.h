#ifndef CAMERAMOVEMENTSYSTEM_H
#define CAMERAMOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/TransformComponent.h"
#include "../../libs/SDL2/SDL.h"
// #include "../Game/Game.h"

/*
This system is responsible for making the camera follow the player by making everything else move in the opposite velocity of the player during movement
*/


class CameraMovementSystem: public System{
    public:
        CameraMovementSystem();

        void Update(SDL_Rect& camera, int mapheight, int mapwidth);
};

#endif 