#ifndef STATUSEFFECTSYSTEM_H
#define STATUSEFFECTSYSTEM_H

#include "../ECS/ECS.h"
#include "../../libs/SDL2/SDL.h"
#include "../Components/StatusEffectComponent.h"
#include "../Events/StatusEffectEvent.h"
#include "../EventBus/EventBus.h"
#include "../Components/BaseStatComponent.h"
#include "../Components/HPMPComponent.h"
#include "../Components/SpeedStatComponent.h"
#include "../Components/OffenseStatComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Events/UpdateDisplayStatEvent.h"
#include "../Utils/colors.h"
#include "../Components/TextLabelComponent.h"
#include "../Components/TransformComponent.h"
#include "../EventBus/EventBus.h"
#include "../AssetStore/AssetStore.h"
#include "../Components/SpriteComponent.h"

/*
This system manages status effects
It is the event handler of StatusEffectEvents to enable them, and it disables expired status effects on update
*/

class StatusEffectSystem: public System{
    private:
        // intentionally, only displays debuffs (felt it would be obnoxious for warriors to be spammed w "berzerk" and "speedy everytime they press space")
        inline void displayStatusEffectText(std::unique_ptr<Registry>& registry, const statuses& statusEnum, const Entity& entity){
            Entity statusText = registry->CreateEntity();
            std::string text;
            switch(statusEnum){
                case QUIET:{
                    text = "QUIET";
                }break;
                case SLOWED:{
                    text = "SLOWED";
                }break;
                case PARALYZE:{
                    text = "PARALYZED";
                }break;
                case BLEEDING:{
                    text = "BLEEDING";
                } break;
                case CONFUSED:{
                    text = "CONFUSED";
                } break;
                case STUNNED:{
                    text = "STUNNED";
                } break;
                case BLIND:{
                    text = "BLIND";
                } break;
                case WEAKENED:{
                    text = "WEAKENED";
                } break;
                case ARMORBROKEN:{
                    text = "ARMOR BROKEN";
                } break;
                default:{
                    return;
                }break;
            }
            statusText.AddComponent<TextLabelComponent>(text, "damagefont", damagered, false, 350, entity.GetId(), entity.GetCreationId());
            statusText.AddComponent<TransformComponent>(entity.GetComponent<TransformComponent>().position);
        }

        std::vector<SDL_Rect> icons = { // can be indexed w/ statuses enums
            {0*8, 2*8, 8, 8}, // quiet
            {1*8, 0*8, 8, 8}, // slowed
            {5*8, 3*8, 8, 8}, // paralyze 
            {0*8, 0*8, 8, 8}, // speedy 
            {2*8, 3*8, 8, 8}, // berserk
            {2*8, 0*8, 8, 8}, // confused
            {14*8, 2*8, 8, 8}, // bleeding
            {1*8, 1*8, 8, 8}, // invulnerable 
            {13*8, 2*8, 8, 8}, // stunned
            {2*8, 1*8, 8, 8}, // invisible
            {8*9, 2*8, 8, 8}, // blind
            {0, 1*8, 8, 8}, // armored
            {15*8, 2*8, 8, 8}, // healing
            {1*8, 3*8, 8, 8}, // damaging
            {2*8, 2*8, 8, 8}, // WEAKENED 
            {7*8, 3*8, 8, 8}, // armor broken
        };

        std::unordered_map<unsigned long, SDL_Texture*> effectTextures;

        void GenerateStatusIcon(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, std::bitset<TOTAL_NUMBER_OF_STATUS_EFFECTS> bits);

    public:
        StatusEffectSystem();

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus);

        void onStatusEnable(StatusEffectEvent& event);

        void onStatusDisable(Entity& recipient, statuses status, std::unique_ptr<EventBus>& eventbus);

        void Update(SDL_Renderer* renderer, std::unique_ptr<EventBus>& eventbus, std::unique_ptr<AssetStore>& assetStore, const SDL_Rect& camera);

        void killIconSetTextures();

};

#endif