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
#include "../Events/SpellUseEvent.h"
#include "../AssetStore/AssetStore.h"
#include "../Components/PlayerItemsComponent.h"
#include "../Components/StatusEffectComponent.h"
#include "../Events/CloakUseEvent.h"
#include "../Events/ShieldUseEvent.h"

/*
This system is responsible for updating the player velocity-direction, sprite, animation, flags, and ability use based off of keyboard input
It can be understood as a system that prepares certain player components for their updates (or events) based off user-input
like an AI system but for the player 
*/

class KeyboardMovementSystem: public System {
    private:
        //given mouse coordiantes and player coordiantes, returns "X-shaped-quadrant" relative to player
        inline sidehit clicked(int mx, int my, int px, int py, SDL_Rect camera){
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

        std::unordered_map<std::bitset<5>, movements> confusedMoves = {
                {std::bitset<5>(0b00000), NONE},
                {std::bitset<5>(0b00001), DOWN},
                {std::bitset<5>(0b00010), RIGHT},
                {std::bitset<5>(0b00011), DOWNRIGHT},
                {std::bitset<5>(0b00100), UP},
                {std::bitset<5>(0b00101), NONE},
                {std::bitset<5>(0b00110), UPRIGHT},
                {std::bitset<5>(0b00111), RIGHT},
                {std::bitset<5>(0b01000), LEFT},
                {std::bitset<5>(0b01001), DOWNLEFT},
                {std::bitset<5>(0b01010), NONE},
                {std::bitset<5>(0b01011), DOWN},
                {std::bitset<5>(0b01100), UPLEFT},
                {std::bitset<5>(0b01101), LEFT},
                {std::bitset<5>(0b01110), UP},
                {std::bitset<5>(0b01111), NONE},
                {std::bitset<5>(0b10000), NONE},
                {std::bitset<5>(0b10001), DOWN},
                {std::bitset<5>(0b10010), RIGHT},
                {std::bitset<5>(0b10011), DOWNRIGHT},
                {std::bitset<5>(0b10100), UP},
                {std::bitset<5>(0b10101), NONE},
                {std::bitset<5>(0b10110), UPRIGHT},
                {std::bitset<5>(0b10111), RIGHT},
                {std::bitset<5>(0b11000), LEFT},
                {std::bitset<5>(0b11001), DOWNLEFT},
                {std::bitset<5>(0b11010), NONE},
                {std::bitset<5>(0b11011), DOWN},
                {std::bitset<5>(0b11100), UPLEFT},
                {std::bitset<5>(0b11101), LEFT},
                {std::bitset<5>(0b11110), UP},
                {std::bitset<5>(0b11111), NONE}
        };

    public:
        KeyboardMovementSystem();

        void Update(const std::bitset<5>& keysPressed, int mouseX, int mouseY, SDL_Rect camera, bool space, std::unique_ptr<AssetStore>& assetStore, std::unique_ptr<EventBus>& eventbus, std::unique_ptr<Registry>& registry);
        
};

#endif