#ifndef KEYBOARDMOVEMENTSYSTEM_H
#define KEYBOARDMOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../EventBus/EventBus.h"
#include "../Components/KeyboardControlledComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BaseStatComponent.h"
#include "../Components/HPMPComponent.h"
#include "../Components/OffenseStatComponent.h"
#include "../Components/SpeedStatComponent.h"
#include "../Components/ClassNameComponent.h"
#include "../Utils/enums.h"
#include "../Components/AnimatedShootingComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/AbilityComponent.h"
#include "../Events/TomeUseEvent.h"
#include "../Events/QuiverUseEvent.h"
#include "../Events/HelmUseEvent.h"

/*
This system is responsible for updating the player velocity-direction, sprite, animation, flags, and ability use based off of keyboard input
It can be understood as a system that prepares certain player components for their updates (or events) based off user-input
*/

class KeyboardMovementSystem: public System {
    private:
        //given mouse coordiantes and player coordiantes, returns "X-shaped-quadrant" relative to player
        sidehit clicked(int mx, int my, int px, int py, SDL_Rect camera){
            mx += camera.x;
            my += camera.y;
            int dx = mx - px;
            int dy = my - py;
            if(abs(dx) > abs(dy)){
                if(dx > 0){
                    return RIGHTSIDE; //right
                }
                else{
                    return LEFTSIDE; //left 
                }   
            }
            else{
                if(dy > 0){
                    return BOTTOMSIDE; //below 
                }
                else {
                    return TOPSIDE; //above
                }
            }
        }

        std::unordered_map<std::bitset<5>, movements> moves = {
                {std::bitset<5>(0b00000), NONE},
                {std::bitset<5>(0b00001), UP},
                {std::bitset<5>(0b00010), LEFT},
                {std::bitset<5>(0b00011), UPLEFT},
                {std::bitset<5>(0b00100), DOWN},
                {std::bitset<5>(0b00101), NONE},
                {std::bitset<5>(0b00110), DOWNLEFT},
                {std::bitset<5>(0b00111), LEFT},
                {std::bitset<5>(0b01000), RIGHT},
                {std::bitset<5>(0b01001), UPRIGHT},
                {std::bitset<5>(0b01010), NONE},
                {std::bitset<5>(0b01011), UP},
                {std::bitset<5>(0b01100), DOWNRIGHT},
                {std::bitset<5>(0b01101), RIGHT},
                {std::bitset<5>(0b01110), DOWN},
                {std::bitset<5>(0b01111), NONE},
                {std::bitset<5>(0b10000), NONE},
                {std::bitset<5>(0b10001), UP},
                {std::bitset<5>(0b10010), LEFT},
                {std::bitset<5>(0b10011), UPLEFT},
                {std::bitset<5>(0b10100), DOWN},
                {std::bitset<5>(0b10101), NONE},
                {std::bitset<5>(0b10110), DOWNLEFT},
                {std::bitset<5>(0b10111), LEFT},
                {std::bitset<5>(0b11000), RIGHT},
                {std::bitset<5>(0b11001), UPRIGHT},
                {std::bitset<5>(0b11010), NONE},
                {std::bitset<5>(0b11011), UP},
                {std::bitset<5>(0b11100), DOWNRIGHT},
                {std::bitset<5>(0b11101), RIGHT},
                {std::bitset<5>(0b11110), DOWN},
                {std::bitset<5>(0b11111), NONE}
        };

    public:
        KeyboardMovementSystem(){
            RequireComponent<KeyboardControlledComponent>(); //only the player has this.
        }

        void Update(const std::bitset<5>& keysPressed, int mouseX, int mouseY, SDL_Rect camera, bool space, std::unique_ptr<AssetStore>& assetStore, std::unique_ptr<EventBus>& eventbus, std::unique_ptr<Registry>& registry){
            if(GetSystemEntities().size() == 0){return;}
            const auto& player = GetSystemEntities()[0]; //asumes this system has 1 entity: the player
            auto& sprite = player.GetComponent<SpriteComponent>();
            auto& rigidbody = player.GetComponent<RidigBodyComponent>();
            auto& animation = player.GetComponent<AnimationComponent>();
            const auto& classname = player.GetComponent<ClassNameComponent>().classname;
            const auto& pec = player.GetComponent<ProjectileEmitterComponent>();

            int move = moves[keysPressed];

            if(space){ // ability use attempt by player
                auto& ac = player.GetComponent<AbilityComponent>(); 
                auto& activemp = player.GetComponent<HPMPComponent>().activemp;
                Uint32 time = SDL_GetTicks();
                if(ac.abilityEquipped && ( ac.timeLastUsed == 0 || time >= ac.timeLastUsed + ac.coolDownMS) ){ // if ability item equipped and not in cooldown
                    if(activemp < ac.mpRequired){ // if not enough mana
                        assetStore->PlaySound(NOMANA);
                    } else { // else, we have enough mana: evoke ability
                        const auto& classname = player.GetComponent<ClassNameComponent>().classname;
                        switch(classname){ 
                            case PRIEST:{
                                auto& HPMP = player.GetComponent<HPMPComponent>();
                                if(HPMP.activehp < HPMP.maxhp){
                                    activemp -= ac.mpRequired;
                                    ac.timeLastUsed = time;
                                    eventbus->EmitEvent<TomeUseEvent>(player, registry, assetStore);
                                }
                                break;}
                            case WARRIOR:{
                                activemp -= ac.mpRequired;
                                ac.timeLastUsed = time;
                                eventbus->EmitEvent<HelmUseEvent>(player, eventbus, registry);
                                break;}
                            case ARCHER:{
                                activemp -= ac.mpRequired;
                                ac.timeLastUsed = time;
                                eventbus->EmitEvent<QuiverUseEvent>(player, registry,mouseX, mouseY, camera);
                                break;}
                        }
                    }
                }
            }

            // update stuff based off direction of travel
            if(move == UP){ // up
                rigidbody.velocity = glm::vec2(0, -1);
                animation.xmin = 1;
                sprite.srcRect.y = sprite.height * 2 + classname * 24;
                sprite.srcRect.x = sprite.height * 2;
                animation.numFrames = 2;
                sprite.flip = SDL_FLIP_NONE;
            }
            else if (move == DOWN) { // down
                rigidbody.velocity = glm::vec2(0, 1);
                animation.xmin = 1;
                sprite.srcRect.y = sprite.height * 1 + classname * 24;
                sprite.srcRect.x = sprite.height * 1;
                animation.numFrames = 2;
                sprite.flip = SDL_FLIP_NONE;
            }
            else if (move == LEFT) { // left
                rigidbody.velocity = glm::vec2(-1, 0);
                animation.xmin = 0;
                sprite.srcRect.y = sprite.height * 0 + classname * 24;
                sprite.srcRect.x = sprite.height * 0;
                animation.numFrames = 2;
                sprite.flip = SDL_FLIP_HORIZONTAL;
            }
            else if (move == RIGHT) { // right
                animation.xmin = 0;
                sprite.srcRect.y = sprite.height * 0 + classname * 24;
                sprite.srcRect.x = sprite.height * 0;
                animation.numFrames = 2;
                rigidbody.velocity = glm::vec2(1, 0);
                sprite.flip = SDL_FLIP_NONE;
            }
            else if (move == UPRIGHT){ // up-right
                rigidbody.velocity = glm::vec2(1, -1);
                animation.xmin = 1;
                sprite.srcRect.y = sprite.height * 2 + classname * 24;
                sprite.srcRect.x = sprite.height * 2;
                animation.numFrames = 2;
                sprite.flip = SDL_FLIP_NONE;
            }
            else if (move == UPLEFT){ // up-left 
                rigidbody.velocity = glm::vec2(-1, -1);
                animation.xmin = 1;
                sprite.srcRect.y = sprite.height * 2 + classname * 24;
                sprite.srcRect.x = sprite.height * 2;
                animation.numFrames = 2;
                sprite.flip = SDL_FLIP_NONE;
            }
            else if (move == DOWNRIGHT){ // down-right
                rigidbody.velocity = glm::vec2(1, 1);
                animation.xmin = 1;
                sprite.srcRect.y = sprite.height * 1 + classname * 24;
                sprite.srcRect.x = sprite.height * 1;
                animation.numFrames = 2;
                sprite.flip = SDL_FLIP_NONE;
            }
            else if (move == DOWNLEFT){ // down-left
                rigidbody.velocity = glm::vec2(-1, 1);
                animation.xmin = 1;
                sprite.srcRect.y = sprite.height * 1 + classname * 24;
                sprite.srcRect.x = sprite.height * 1;
                animation.numFrames = 2;
                sprite.flip = SDL_FLIP_NONE;

            }
            else if (move == NONE){ // none
                animation.numFrames = 1;
                animation.currentFrame = 1;
                animation.xmin = 0;
                rigidbody.velocity = glm::vec2(0);
            }

            // update stuff for if shooting
            if(pec.isShooting && pec.shots > 0){ //previously if keysPressed[4]
                const auto& activedexterity = player.GetComponent<OffenseStatComponent>().activedexterity;
                auto& transform = player.GetComponent<TransformComponent>();
                auto& asc = player.GetComponent<AnimatedShootingComponent>();
                int playerCenterX = transform.position.x + ( (sprite.width * transform.scale.x) / 2);
                int playerCenterY = transform.position.y + sprite.height * transform.scale.y;
                int mousePos = clicked(mouseX, mouseY, playerCenterX, playerCenterY, camera);
                if(mousePos == RIGHTSIDE){ // right
                    sprite.srcRect.y = sprite.height * 0 + classname * 24;
                    asc.animatedShooting = true;
                    sprite.flip = SDL_FLIP_NONE;
                } else if (mousePos == BOTTOMSIDE){ // below 
                    asc.animatedShooting = false;
                    sprite.flip = SDL_FLIP_NONE;
                    sprite.srcRect.y = sprite.height * 1 + classname * 24;
                } else if (mousePos == TOPSIDE) { // above
                    sprite.srcRect.y = sprite.height * 2 + classname * 24;
                    asc.animatedShooting = false;
                    sprite.flip = SDL_FLIP_NONE;
                } else { // mousePos == "left"
                    sprite.srcRect.y = sprite.height * 0 + classname * 24;
                    sprite.flip = SDL_FLIP_HORIZONTAL;
                    asc.animatedShooting = true;
                }
                animation.numFrames = 2;
                animation.xmin = 4;
                animation.frameSpeedRate = (.08666 * activedexterity + 1.5) * 2;
            } else {
                const auto& activespeed = player.GetComponent<SpeedStatComponent>().activespeed;
                auto& asc = player.GetComponent<AnimatedShootingComponent>();
                animation.frameSpeedRate = activespeed/10;
                asc.animatedShooting = false;
            }

        }
        
};

#endif