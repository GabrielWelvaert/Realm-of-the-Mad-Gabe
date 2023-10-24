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
                default:{
                    return;
                }break;
            }
            statusText.AddComponent<TextLabelComponent>(text, "damagefont", damagered, false, 350, entity.GetId(), entity.GetCreationId());
            statusText.AddComponent<TransformComponent>(entity.GetComponent<TransformComponent>().position);
        }

    public:
        StatusEffectSystem();

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus);

        void onStatusEnable(StatusEffectEvent& event);

        void onStatusDisable(Entity& recipient, const int& statusEnum, std::unique_ptr<EventBus>& eventbus);

        void Update(std::unique_ptr<EventBus>& eventbus);

};

#endif