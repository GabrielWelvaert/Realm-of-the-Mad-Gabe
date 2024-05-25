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
#include "../Components/isShootingComponent.h"
#include "../Events/CloakUseEvent.h"
#include "../Events/ShieldUseEvent.h"
#include "../Utils/KeyBoardInput.h"

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

        std::unordered_map<std::bitset<8>, movements> moves = {
                {std::bitset<8>(0b00000000), NONE},
                {std::bitset<8>(0b00000001), UP},
                {std::bitset<8>(0b00000010), LEFT},
                {std::bitset<8>(0b00000011), UPLEFT},
                {std::bitset<8>(0b00000100), DOWN},
                {std::bitset<8>(0b00000101), NONE},
                {std::bitset<8>(0b00000110), DOWNLEFT},
                {std::bitset<8>(0b00000111), LEFT},
                {std::bitset<8>(0b00001000), RIGHT},
                {std::bitset<8>(0b00001001), UPRIGHT},
                {std::bitset<8>(0b00001010), NONE},
                {std::bitset<8>(0b00001011), UP},
                {std::bitset<8>(0b00001100), DOWNRIGHT},
                {std::bitset<8>(0b00001101), RIGHT},
                {std::bitset<8>(0b00001110), DOWN},
                {std::bitset<8>(0b00001111), NONE},
                {std::bitset<8>(0b00010000), NONE},
                {std::bitset<8>(0b00010001), UP},
                {std::bitset<8>(0b00010010), LEFT},
                {std::bitset<8>(0b00010011), UPLEFT},
                {std::bitset<8>(0b00010100), DOWN},
                {std::bitset<8>(0b00010101), NONE},
                {std::bitset<8>(0b00010110), DOWNLEFT},
                {std::bitset<8>(0b00010111), LEFT},
                {std::bitset<8>(0b00011000), RIGHT},
                {std::bitset<8>(0b00011001), UPRIGHT},
                {std::bitset<8>(0b00011010), NONE},
                {std::bitset<8>(0b00011011), UP},
                {std::bitset<8>(0b00011100), DOWNRIGHT},
                {std::bitset<8>(0b00011101), RIGHT},
                {std::bitset<8>(0b00011110), DOWN},
                {std::bitset<8>(0b00011111), NONE}
        };

        std::unordered_map<std::bitset<8>, movements> confusedMoves = {
                {std::bitset<8>(0b00000000), NONE},
                {std::bitset<8>(0b00000001), DOWN},
                {std::bitset<8>(0b00000010), RIGHT},
                {std::bitset<8>(0b00000011), DOWNRIGHT},
                {std::bitset<8>(0b00000100), UP},
                {std::bitset<8>(0b00000101), NONE},
                {std::bitset<8>(0b00000110), UPRIGHT},
                {std::bitset<8>(0b00000111), RIGHT},
                {std::bitset<8>(0b00001000), LEFT},
                {std::bitset<8>(0b00001001), DOWNLEFT},
                {std::bitset<8>(0b00001010), NONE},
                {std::bitset<8>(0b00001011), DOWN},
                {std::bitset<8>(0b00001100), UPLEFT},
                {std::bitset<8>(0b00001101), LEFT},
                {std::bitset<8>(0b00001110), UP},
                {std::bitset<8>(0b00001111), NONE},
                {std::bitset<8>(0b00010000), NONE},
                {std::bitset<8>(0b00010001), DOWN},
                {std::bitset<8>(0b00010010), RIGHT},
                {std::bitset<8>(0b00010011), DOWNRIGHT},
                {std::bitset<8>(0b00010100), UP},
                {std::bitset<8>(0b00010101), NONE},
                {std::bitset<8>(0b00010110), UPRIGHT},
                {std::bitset<8>(0b00010111), RIGHT},
                {std::bitset<8>(0b00011000), LEFT},
                {std::bitset<8>(0b00011001), DOWNLEFT},
                {std::bitset<8>(0b00011010), NONE},
                {std::bitset<8>(0b00011011), DOWN},
                {std::bitset<8>(0b00011100), UPLEFT},
                {std::bitset<8>(0b00011101), LEFT},
                {std::bitset<8>(0b00011110), UP},
                {std::bitset<8>(0b00011111), NONE}
        };

    public:
        KeyboardMovementSystem();

        void Update(std::unique_ptr<KeyBoardInput>& keyboardinput, int mouseX, int mouseY, SDL_Rect camera, std::unique_ptr<AssetStore>& assetStore, std::unique_ptr<EventBus>& eventbus, std::unique_ptr<Registry>& registry);
        
};

#endif