#ifndef UPDATEDISPLAYSTATTEXTSYSTEM_H
#define UPDATEDISPLAYSTATTEXTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/DisplayStatComponent.h"
#include "../Components/BaseStatComponent.h"
#include "../Components/ClassNameComponent.h"
#include "../Utils/enums.h"
#include "../EventBus/EventBus.h"
#include "../Events/UpdateDisplayStatEvent.h"
#include "../Components/HPMPComponent.h"
#include "../Components/SpeedStatComponent.h"
#include "../Components/OffenseStatComponent.h"
#include "../../libs/SDL2/SDL.h"
#include "../../libs/SDL2/SDL_image.h"
#include "../../libs/SDL2/SDL_ttf.h"

/*
This system updates the display strings for xp, hp, and mp. They are only displayed and updated when mouse pos > 750
They are rendered in RenderTextSystem
*/

class UpdateDisplayStatTextSystem: public System{
    private:
        
    public:
        UpdateDisplayStatTextSystem(){
            RequireComponent<DisplayStatComponent>();
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
            eventBus->SubscribeToEvent<UpdateDisplayStatEvent>(this, &UpdateDisplayStatTextSystem::onDisplayStatUpdate);
        }

        void onUpdateDisplayName(){
            //todo
        }

        void sort(){
            auto& entities = GetSystemEntities();
            std::sort(entities.begin(), entities.end(), [](const Entity& entity1, const Entity& entity2) {return entity1.GetComponent<DisplayStatComponent>().statEnum < entity2.GetComponent<DisplayStatComponent>().statEnum;});
        }

        // event update for att,def,spd,dex,vit,wis and display string
        void onDisplayStatUpdate(UpdateDisplayStatEvent& event){
            auto& entities = GetSystemEntities();
            if(entities.size() == 0){return;}
            const auto& pbs = event.player.GetComponent<BaseStatComponent>();
            const auto& hpmp = event.player.GetComponent<HPMPComponent>();
            const auto& offenseStats = event.player.GetComponent<OffenseStatComponent>();
            const auto& activeSpeedStat = event.player.GetComponent<SpeedStatComponent>().activespeed;
            const auto& classname = event.player.GetComponent<ClassNameComponent>().classname;

            SDL_Color color;

            auto& attDisplayText = entities[ATTACK].GetComponent<TextLabelComponent>();
            // std::cout << "attDisplay text has id " << entities[ATTACK].GetId() << " in UpdateDisplayStatTextSystem" << std::endl;
            std::string attdisplayString = std::to_string(offenseStats.activeattack);
            color = grey;
            if(offenseStats.activeattack > pbs.attack){
                attdisplayString += " (+" + std::to_string(offenseStats.activeattack - pbs.attack) + ")";
                color = statgreen;
            }
            if(pbs.attack == maxStats[classname][ATTACK]){
                color = maxstatcolor;
            }
            attDisplayText.text = attdisplayString;
            attDisplayText.color = color;
            attDisplayText.spawnframe = true;

            // std::cout << "attDisplay text displaying " << attDisplayText.text << " at " << entities[ATTACK].GetComponent<TransformComponent>().position.x << ", " << entities[ATTACK].GetComponent<TransformComponent>().position.y << std::endl;

            auto& defDisplayText = entities[DEFENSE].GetComponent<TextLabelComponent>();
            std::string defdisplayString = std::to_string(hpmp.activedefense);
            color = grey;
            if(hpmp.activedefense > pbs.defense){
                defdisplayString += " (+" + std::to_string(hpmp.activedefense - pbs.defense) + ")";
                color = statgreen;
            }
            if(pbs.defense == maxStats[classname][DEFENSE]){
                color = maxstatcolor;
            }
            defDisplayText.text = defdisplayString;
            defDisplayText.color = color;
            defDisplayText.spawnframe = true;

            auto& spdDisplayText = entities[SPEED].GetComponent<TextLabelComponent>();
            std::string spddisplayString = std::to_string(activeSpeedStat);
            color = grey;
            if(activeSpeedStat > pbs.speed){
                spddisplayString += " (+" + std::to_string(activeSpeedStat - pbs.speed) + ")";
                color = statgreen;
            }
            if(pbs.speed == maxStats[classname][SPEED]){
                color = maxstatcolor;
            }
            spdDisplayText.text = spddisplayString;
            spdDisplayText.color = color;
            spdDisplayText.spawnframe = true;

            auto& dexDisplayText = entities[DEXTERITY].GetComponent<TextLabelComponent>();
            std::string dexdisplayString = std::to_string(offenseStats.activedexterity);
            color = grey;
            if(offenseStats.activedexterity > pbs.dexterity){
                dexdisplayString += " (+" + std::to_string(offenseStats.activedexterity - pbs.dexterity) + ")";
                color = statgreen;
            }
            if(pbs.dexterity == maxStats[classname][DEXTERITY]){
                color = maxstatcolor;
            }
            dexDisplayText.text = dexdisplayString;
            dexDisplayText.color = color;
            dexDisplayText.spawnframe = true;

            auto& vitDisplayText = entities[VITALITY].GetComponent<TextLabelComponent>();
            std::string vitdisplayString = std::to_string(hpmp.activevitality);
            color = grey;
            if(hpmp.activevitality > pbs.vitality){
                vitdisplayString += " (+" + std::to_string(hpmp.activevitality - pbs.vitality) + ")";
                color = statgreen;
            }
            if(pbs.vitality == maxStats[classname][VITALITY]){
                color = maxstatcolor;
            }
            vitDisplayText.text = vitdisplayString;
            vitDisplayText.color = color;
            vitDisplayText.spawnframe = true;

            auto& wisDisplayText = entities[WISDOM].GetComponent<TextLabelComponent>();
            std::string wisdisplayString = std::to_string(hpmp.activewisdom);
            color = grey;
            if(hpmp.activewisdom > pbs.wisdom){
                wisdisplayString += " (+" + std::to_string(hpmp.activewisdom - pbs.wisdom) + ")";
                color = statgreen;
            }
            if(pbs.wisdom == maxStats[classname][WISDOM]){
                color = maxstatcolor;
            }
            wisDisplayText.text = wisdisplayString;
            wisDisplayText.color = color;
            wisDisplayText.spawnframe = true;

            entities[LVL].GetComponent<TextLabelComponent>().text = "Lvl " + std::to_string(static_cast<int>(pbs.level));
            entities[LVL].GetComponent<TextLabelComponent>().spawnframe = true;

        }

        // system update each frame if mousepos > 750 for xp, hp, mp display string
        void Update(int mx, int my, Entity player, std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer){
            auto entities = GetSystemEntities();
            if(entities.size() == 0){return;}
            if (mx > 750 && mx < 1000 && my > 295 && my < 370){ // if mouse is hovering over the stat bars
                // std::sort(entities.begin(), entities.end(), [](const Entity& entity1, const Entity& entity2) {return entity1.GetComponent<DisplayStatComponent>().statEnum < entity2.GetComponent<DisplayStatComponent>().statEnum;});
                auto& hpdisplay = entities[HP];
                auto& mpdisplay = entities[MP];
                auto& xpdisplay = entities[XP];
                auto& hptext = hpdisplay.GetComponent<TextLabelComponent>();
                auto& mptext = mpdisplay.GetComponent<TextLabelComponent>();
                auto& xptext = xpdisplay.GetComponent<TextLabelComponent>();
                hptext.invisible = mptext.invisible = xptext.invisible = false;
                const auto& pbs = player.GetComponent<BaseStatComponent>();
                const auto& hpmp = player.GetComponent<HPMPComponent>();
                const auto& classname = player.GetComponent<ClassNameComponent>().classname;
                hptext.text = std::to_string(static_cast<int>(hpmp.activehp)) + "/" + std::to_string(hpmp.maxhp);
                mptext.text  = std::to_string(static_cast<int>(hpmp.activemp)) + "/" + std::to_string(hpmp.maxmp);
                if(pbs.level < 20){
                    xptext.text = std::to_string(pbs.xp - nextXPToLevelUp[pbs.level-1]) + "/" + std::to_string(nextXPToLevelUp[pbs.level] - nextXPToLevelUp[pbs.level-1]);
                } else {
                    xptext.text = std::to_string(pbs.xp);
                }

                auto& lasthp = hpdisplay.GetComponent<DisplayStatComponent>().lastDisplayed;
                auto& lastmp = mpdisplay.GetComponent<DisplayStatComponent>().lastDisplayed;
                auto& lastxp = xpdisplay.GetComponent<DisplayStatComponent>().lastDisplayed;

                if(hptext.text != lasthp || mptext.text != lastmp || xptext.text != lastxp){
                    pbs.hp == maxStats[classname][HP] ? hptext.color = maxstatcolor : hptext.color = white;
                    pbs.mp == maxStats[classname][MP] ? mptext.color = maxstatcolor : hptext.color = white;

                    auto font = assetStore->GetFont(mptext.assetId);

                    TTF_SizeText(font, hptext.text.c_str(), &hptext.textwidth, NULL);
                    TTF_SizeText(font, mptext.text.c_str(), &mptext.textwidth, NULL);
                    TTF_SizeText(font, xptext.text.c_str(), &xptext.textwidth, NULL);
                    hpdisplay.GetComponent<TransformComponent>().position.x = 877.5 - 0.5 * hptext.textwidth;
                    mpdisplay.GetComponent<TransformComponent>().position.x = 877.5 - 0.5 * mptext.textwidth;
                    xpdisplay.GetComponent<TransformComponent>().position.x = 877.5 - 0.5 * xptext.textwidth;
                    lasthp = hptext.text;
                    lastmp = mptext.text;
                    lastxp = xptext.text;
                }
            } else {
                auto& hpdisplay = entities[HP];
                auto& mpdisplay = entities[MP];
                auto& xpdisplay = entities[XP];
                auto& hptext = hpdisplay.GetComponent<TextLabelComponent>();
                auto& mptext = mpdisplay.GetComponent<TextLabelComponent>();
                auto& xptext = xpdisplay.GetComponent<TextLabelComponent>();
                // std::cout << xptext.color.r << ", " << xptext.color.g << ", " << xptext.color.b << std::endl;
                hptext.invisible = true;
                mptext.invisible = true;
                xptext.invisible = true;
                return;
            }
        }


};




#endif