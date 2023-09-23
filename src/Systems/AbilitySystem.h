#pragma once 

#include "../ECS/ECS.h"
#include "../Components/AbilityComponent.h"
#include "../Events/EquipAbilityEvent.h"
#include "../EventBus/EventBus.h"
#include "../Components/ClassNameComponent.h"
#include "../Utils/tables.h"
#include "../Events/TomeUseEvent.h"

/*
This system is responsible for being the event handler for equipping abilities and using them!
*/

class AbilitySytem: public System{
    public:
        AbilitySytem(){
            RequireComponent<AbilityComponent>();
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
            eventBus->SubscribeToEvent<EquipAbilityEvent>(this, &AbilitySytem::onAbilityEquip);
            eventBus->SubscribeToEvent<TomeUseEvent>(this, &AbilitySytem::onTomeUse);
        }

        void onTomeUse(TomeUseEvent& event){
            // at this point we've done everything (check tome, mp, subtract mp) except use the tome, so use it!
            auto& HPMP = event.player.GetComponent<HPMPComponent>();
            const auto& tome = event.player.GetComponent<TomeComponent>();
            HPMP.activehp += tome.hp;
            if(HPMP.activehp > HPMP.maxhp){
                HPMP.activehp = HPMP.maxhp;
            }
        }

        void onAbilityEquip(EquipAbilityEvent& event){
            std::cout << " abiltiy equip event handled! " << std::endl;
            auto& player = event.player;
            auto& classname = player.GetComponent<ClassNameComponent>().classname;
            auto& ac = player.GetComponent<AbilityComponent>();
            auto newAbilityData = itemEnumToAbilityData.at(event.itemEnum);
            ac.coolDownMS = newAbilityData.cooldown;
            ac.mpRequired = newAbilityData.mprequired;
            ac.abilityEquipped = true;

            switch(classname){
                case ARCHER:{
                    auto& quiver = player.GetComponent<QuiverComponent>();
                    //todo update quiver data 
                    break;
                }
                case PRIEST: {
                    auto& tome = player.GetComponent<TomeComponent>();
                    tome.hp = itemEnumToTomeData.at(event.itemEnum).hp;
                    break;
                }
                case WARRIOR: {
                    auto& helm = player.GetComponent<HelmComponent>();
                    break;
                }
            }

        }


};