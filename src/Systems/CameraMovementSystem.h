#ifndef CAMERAMOVEMENTSYSTEM_H
#define CAMERAMOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/TransformComponent.h"
#include "../../libs/SDL2/SDL.h"
#include "../Game/Game.h"

/*
This system is responsible for making the camera follow the player by making everything else move in the opposite velocity of the player during movement
*/


class CameraMovementSystem: public System{
    public:
        CameraMovementSystem(){
            RequireComponent<CameraFollowComponent>();
            RequireComponent<TransformComponent>();
        }

        void Update(SDL_Rect& camera){
            for(auto entity: GetSystemEntities()){
                const auto& transform = entity.GetComponent<TransformComponent>();

                // change camera x and y based off of entity transform x y
                if (transform.position.x + (camera.w/2) < Game::mapheight){ // previoulsy < Game::mapwidth 
                    camera.x = transform.position.x - (camera.w/2 - 20);
                }
                if (transform.position.y + (camera.h/2) < Game::mapheight){ // previoulsy < Game::mapwidth 
                    camera.y = transform.position.y - (camera.h/2 - 20);
                }
                camera.x = camera.x < 0 ? 0 : camera.x; // if cam.x < 0 make it 0 otherwise leave it. 
                camera.y = camera.y < 0 ? 0 : camera.y;
                camera.x = camera.x > camera.w ? camera.w : camera.x;
                camera.y = camera.y > camera.h ? camera.h : camera.y;

            }
        }
};

#endif 