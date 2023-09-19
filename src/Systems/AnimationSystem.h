#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include "../../libs/SDL2/SDL.h"
#include "../../libs/SDL2/SDL_image.h"
#include "../ECS/ECS.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../AssetStore/AssetStore.h"
#include <algorithm>

/*
This system is reponsible for updating the sprite source rect for entities given the amount of deltatime that has passed
*/

class AnimationSystem: public System {

    public:
        AnimationSystem(){
            RequireComponent<SpriteComponent>();
            RequireComponent<AnimationComponent>();
            RequireComponent<TransformComponent>();
        }

        void Update(SDL_Rect camera){
            for(auto entity: GetSystemEntities()){
                auto& animation = entity.GetComponent<AnimationComponent>();
                auto& sprite = entity.GetComponent<SpriteComponent>();
                const auto& transform = entity.GetComponent<TransformComponent>();

                // if entity is off-camera, no need to update 
                if(transform.position.x + sprite.width * transform.scale.x < camera.x || 
                   transform.position.y + sprite.height * transform.scale.y < camera.y || 
                   transform.position.x > camera.x + camera.w || 
                   transform.position.y > camera.y + camera.h){ 
                   continue;
                } 

                double elapsedSeconds = static_cast<double>(SDL_GetTicks() - animation.startTime) / 1000.0;
                double frameIndexFloat = elapsedSeconds * animation.frameSpeedRate;
                animation.currentFrame = static_cast<unsigned int>(frameIndexFloat) % animation.numFrames;
                sprite.srcRect.x = animation.currentFrame * sprite.width + (sprite.width * animation.xmin);


            }
        }
};

#endif