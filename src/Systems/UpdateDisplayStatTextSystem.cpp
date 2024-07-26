#include "UpdateDisplayStatTextSystem.h"

UpdateDisplayStatTextSystem::UpdateDisplayStatTextSystem(){
    RequireComponent<DisplayStatComponent>();
    RequireComponent<TextLabelComponent>();
    RequireComponent<TransformComponent>();
}

void UpdateDisplayStatTextSystem::SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
    eventBus->SubscribeToEvent<UpdateDisplayStatEvent>(this, &UpdateDisplayStatTextSystem::onDisplayStatUpdate);
}


void UpdateDisplayStatTextSystem::onUpdateDisplayName(){}

void UpdateDisplayStatTextSystem::sortEntities(){
    auto& entities = GetSystemEntities();
    std::sort(entities.begin(), entities.end(), [](const Entity& entity1, const Entity& entity2) {return entity1.GetComponent<DisplayStatComponent>().statEnum < entity2.GetComponent<DisplayStatComponent>().statEnum;});
}

void UpdateDisplayStatTextSystem::onDisplayStatUpdate(UpdateDisplayStatEvent& event){

    auto& entities = GetSystemEntities();
    if(entities.size() == 0){return;}
    const auto& pbs = event.player.GetComponent<BaseStatComponent>();
    const auto& hpmp = event.player.GetComponent<HPMPComponent>();
    const auto& offenseStats = event.player.GetComponent<OffenseStatComponent>();
    const auto& activeSpeedStat = event.player.GetComponent<SpeedStatComponent>().activespeed;
    const auto& classname = event.player.GetComponent<ClassNameComponent>().classname;
    const auto& sec = event.player.GetComponent<StatusEffectComponent>();

    SDL_Color color;

    auto& attDisplayText = entities[ATTACK].GetComponent<TextLabelComponent>();
    // std::cout << "attDisplay text has id " << entities[ATTACK].GetId() << " in UpdateDisplayStatTextSystem" << std::endl;
    std::string attdisplayString = std::to_string(offenseStats.activeattack);
    color = grey;
    if(offenseStats.activeattack > pbs.attack){
        attdisplayString += " (+" + std::to_string(offenseStats.activeattack - pbs.attack) + ")";
        color = statgreen;
    } else if(offenseStats.activeattack < pbs.attack){
        attdisplayString += " (-" + std::to_string(pbs.attack - offenseStats.activeattack) + ")";
        color = titleRed;
    }
    if(pbs.attack == maxStats[classname][ATTACK]){
        color = maxstatcolor;
    }
    attDisplayText.text = attdisplayString;
    attDisplayText.color = color;
    attDisplayText.spawnframe = true;

    // std::cout << "attDisplay text displaying " << attDisplayText.text << " at " << entities[ATTACK].GetComponent<TransformComponent>().position.x << ", " << entities[ATTACK].GetComponent<TransformComponent>().position.y << std::endl;

    auto& defDisplayText = entities[DEFENSE].GetComponent<TextLabelComponent>();
    // std::cout << "defDisplayText text has id " << entities[DEFENSE].GetId() << " in UpdateDisplayStatTextSystem" << std::endl;
    std::string defdisplayString = std::to_string(hpmp.activedefense);
    color = grey;
    if(hpmp.activedefense > pbs.defense){
        if(pbs.defense < 2 && sec.effects[ARMORED]){
            defdisplayString += " (+2)";    
        } else {
            defdisplayString += " (+" + std::to_string(hpmp.activedefense - pbs.defense) + ")";
        }
        color = statgreen;
    } else if(hpmp.activedefense == 0 && sec.effects[ARMORBROKEN]){
        if(sec.modifications[ARMORBROKEN] != 0){ // already have 0 def
            defdisplayString += " (-" + std::to_string(sec.modifications[ARMORBROKEN]) +")";    
        }
        color = titleRed;    
        // there is currently no debuff that halves defense. only armor broken exists so there is no active < base logic
    }
    if(pbs.defense == maxStats[classname][DEFENSE]){
        color = maxstatcolor;
    }
    defDisplayText.text = defdisplayString;
    defDisplayText.color = color;
    defDisplayText.spawnframe = true;

    auto& spdDisplayText = entities[SPEED].GetComponent<TextLabelComponent>();
    // std::cout << "spdDisplayText text has id " << entities[SPEED].GetId() << " in UpdateDisplayStatTextSystem" << std::endl;
    std::string spddisplayString = std::to_string(activeSpeedStat);
    color = grey;
    if(activeSpeedStat > pbs.speed){
        spddisplayString += " (+" + std::to_string(activeSpeedStat - pbs.speed) + ")";
        color = statgreen;
    } else if(activeSpeedStat < pbs.speed){ // slowed or paralyzed
        if(sec.effects[PARALYZE]){
            spddisplayString += " (-" + std::to_string(sec.modifications[PARALYZE]) +")";   
        } else { // slowed
            spddisplayString += " (-" + std::to_string(pbs.speed - activeSpeedStat) + ")";
        }
        color = titleRed;
    }
    if(pbs.speed == maxStats[classname][SPEED]){
        color = maxstatcolor;
    }
    spdDisplayText.text = spddisplayString;
    spdDisplayText.color = color;
    spdDisplayText.spawnframe = true;

    auto& dexDisplayText = entities[DEXTERITY].GetComponent<TextLabelComponent>();
    // std::cout << "dexDisplayText text has id " << entities[DEXTERITY].GetId() << " in UpdateDisplayStatTextSystem" << std::endl;
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
    // std::cout << "vitDisplayText text has id " << entities[VITALITY].GetId() << " in UpdateDisplayStatTextSystem" << std::endl;
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
    // std::cout << "wisDisplayText text has id " << entities[WISDOM].GetId() << " in UpdateDisplayStatTextSystem" << std::endl;
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

    // std::cout << "lvl text has id " << entities[LVL].GetId() << " in UpdateDisplayStatTextSystem" << '\n' << '\n';
    entities[LVL].GetComponent<TextLabelComponent>().text = "Lvl " + std::to_string(static_cast<int>(pbs.level));
    entities[LVL].GetComponent<TextLabelComponent>().spawnframe = true;

}

void UpdateDisplayStatTextSystem::checkMaxHPMP(Entity player){
    auto& entities = GetSystemEntities(); // can be indexed with enum stats to get respective entity
    // if(entities.size() == 0){return;}
    const auto& pbs = player.GetComponent<BaseStatComponent>();
    const auto& classname = player.GetComponent<ClassNameComponent>().classname;
    auto& hpdisplay = entities[HP];
    auto& mpdisplay = entities[MP];
    auto& hptext = hpdisplay.GetComponent<TextLabelComponent>();
    auto& mptext = mpdisplay.GetComponent<TextLabelComponent>();
    static_cast<int>(pbs.hp) == maxStats[classname][HP] ? hptext.color = maxstatcolor : hptext.color = white;
    static_cast<int>(pbs.mp) == maxStats[classname][MP] ? mptext.color = maxstatcolor : mptext.color = white;
}

void UpdateDisplayStatTextSystem::Update(int mx, int my, Entity player, std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer, std::unique_ptr<EventBus>& eventBus){
    auto& entities = GetSystemEntities(); // can be indexed with enum stats to get respective entity
    // if(entities.size() == 0){
    //     std::cout << "update display stat text system had 0 entities " << '\n';
    //     return;
    // }
    auto& hpdisplay = entities[HP];
    auto& mpdisplay = entities[MP];
    auto& xpdisplay = entities[XP];
    auto& hptext = hpdisplay.GetComponent<TextLabelComponent>();
    auto& mptext = mpdisplay.GetComponent<TextLabelComponent>();
    auto& xptext = xpdisplay.GetComponent<TextLabelComponent>();
    auto& lasthp = hpdisplay.GetComponent<DisplayStatComponent>().lastDisplayed;
    auto& lastmp = mpdisplay.GetComponent<DisplayStatComponent>().lastDisplayed;
    auto& lastxp = xpdisplay.GetComponent<DisplayStatComponent>().lastDisplayed;

    auto& attackEntity = entities[ATTACK];
    auto& attacktlc = attackEntity.GetComponent<TextLabelComponent>();
    auto& attackDisplayStat = attackEntity.GetComponent<DisplayStatComponent>();

    auto& defenseEntity = entities[DEFENSE];
    auto& defensetlc = defenseEntity.GetComponent<TextLabelComponent>();
    auto& defenseDisplayStat = defenseEntity.GetComponent<DisplayStatComponent>();

    auto& dexterityEntity = entities[DEXTERITY];
    auto& dexteritytlc = dexterityEntity.GetComponent<TextLabelComponent>();
    auto& dexterityDisplayStat = dexterityEntity.GetComponent<DisplayStatComponent>();

    auto& speedEntity = entities[SPEED];
    auto& speedtlc = speedEntity.GetComponent<TextLabelComponent>();
    auto& speedDisplayStat = speedEntity.GetComponent<DisplayStatComponent>();

    auto& wisdomEntity = entities[WISDOM];
    auto& wisdomtlc = wisdomEntity.GetComponent<TextLabelComponent>();
    auto& wisdomDisplayStat = wisdomEntity.GetComponent<DisplayStatComponent>();

    auto& vitalityEntity = entities[VITALITY];
    auto& vitalitytlc = vitalityEntity.GetComponent<TextLabelComponent>();
    auto& vitalityDisplayStat = vitalityEntity.GetComponent<DisplayStatComponent>();

    if(mx > 750 && mx < 1000 && my < 450 && my > 295){ // mouse hovering in HP/MP/XP bars or ATT/DEF/SPD/DEX/VIT/WIS area
        const auto& pbs = player.GetComponent<BaseStatComponent>();
        const auto& hpmp = player.GetComponent<HPMPComponent>();
        const auto& classname = player.GetComponent<ClassNameComponent>().classname;
        if(my < 370){ // mouse hovering over HP/MP/XP bars -- display active out of max
            if(wasDisplayingPotsToMaxLastFrame){
                wasDisplayingPotsToMaxLastFrame = false;
                eventBus->EmitEvent<UpdateDisplayStatEvent>(player);

                // must set att/def/... text.spawnFrame back to true so their sizes may be re-queried in renderTextSystem
                attacktlc.spawnframe = true;
            }
            hptext.invisible = mptext.invisible = xptext.invisible = false;
            hptext.text = std::to_string(static_cast<int>(hpmp.activehp)) + " / " + std::to_string(hpmp.maxhp);
            if(hpmp.maxhp > pbs.hp){
                hptext.text.append(" (+" + std::to_string(static_cast<int>(hpmp.maxhp)-static_cast<int>(pbs.hp)) + ")");
            }
            mptext.text  = std::to_string(static_cast<int>(hpmp.activemp)) + " / " + std::to_string(hpmp.maxmp);
            if(hpmp.maxmp > pbs.mp){
                mptext.text.append(" (+" + std::to_string(static_cast<int>(hpmp.maxmp)-static_cast<int>(pbs.mp)) + ")");
            }
            if(pbs.level < 20){
                xptext.text = std::to_string(pbs.xp - nextXPToLevelUp[pbs.level-1]) + " / " + std::to_string(nextXPToLevelUp[pbs.level] - nextXPToLevelUp[pbs.level-1]);
            } else {
                xptext.text = std::to_string(pbs.xp);
            }

            auto& lasthp = hpdisplay.GetComponent<DisplayStatComponent>().lastDisplayed;
            auto& lastmp = mpdisplay.GetComponent<DisplayStatComponent>().lastDisplayed;
            auto& lastxp = xpdisplay.GetComponent<DisplayStatComponent>().lastDisplayed;

            if(hptext.text != lasthp || mptext.text != lastmp || xptext.text != lastxp){
                pbs.hp == maxStats[classname][HP] ? hptext.color = maxstatcolor : hptext.color = white;
                pbs.mp == maxStats[classname][MP] ? mptext.color = maxstatcolor : mptext.color = white;

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
        } else { // mouse hovering over ATT/DEF/SPD/DEX/VIT/WIS -- display base out of max possible base 
            if(!wasDisplayingPotsToMaxLastFrame){
                wasDisplayingPotsToMaxLastFrame = true;  
                attackDisplayStat.lastDisplayed = ""; // ensure that we update display text
                defenseDisplayStat.lastDisplayed = "";
                speedDisplayStat.lastDisplayed = "";
                dexterityDisplayStat.lastDisplayed = "";
                wisdomDisplayStat.lastDisplayed = "";
                vitalityDisplayStat.lastDisplayed = "";
            }

            hptext.invisible = mptext.invisible = false;
            xptext.invisible = true;

            std::string pbshpstr = std::to_string(pbs.hp);
            if(lasthp != pbshpstr){
                if(pbs.hp == maxStats[classname][HP]){
                    hptext.color = maxstatcolor;
                } else {
                    hptext.color = white;
                }
                hptext.text = pbshpstr + " / " + std::to_string(maxStats[classname][HP]);
                TTF_SizeText(assetStore->GetFont(hptext.assetId), hptext.text.c_str(), &hptext.textwidth, NULL);
                hpdisplay.GetComponent<TransformComponent>().position.x = 877.5 - 0.5 * hptext.textwidth;
                lasthp = pbshpstr;
            }

            std::string pbsmpstr = std::to_string(pbs.mp);
            if(lastmp != pbsmpstr){
                if(pbs.mp == maxStats[classname][MP]){
                    mptext.color = maxstatcolor;
                } else {
                    mptext.color = white;
                }
                mptext.text = pbsmpstr + " / " + std::to_string(maxStats[classname][MP]);
                TTF_SizeText(assetStore->GetFont(mptext.assetId), mptext.text.c_str(), &mptext.textwidth, NULL);
                mpdisplay.GetComponent<TransformComponent>().position.x = 877.5 - 0.5 * mptext.textwidth;
                lastmp = pbsmpstr;
            }

            std::string pbsAttackStr = std::to_string(pbs.attack);
            if(attackDisplayStat.lastDisplayed != pbsAttackStr){
                if(pbs.attack == maxStats[classname][ATTACK]){
                    attacktlc.color = maxstatcolor; 
                } else {
                    attacktlc.color = grey;    
                }
                attacktlc.text = pbsAttackStr + " / " + std::to_string(maxStats[classname][ATTACK]);
                attackDisplayStat.lastDisplayed = pbsAttackStr;
                attacktlc.spawnframe = true; // so renderTextSystem can fix size
            }

            std::string pbsdefenseStr = std::to_string(pbs.defense);
            if(defenseDisplayStat.lastDisplayed != pbsdefenseStr){
                if(pbs.defense == maxStats[classname][DEFENSE]){
                    defensetlc.color = maxstatcolor; 
                } else {
                    defensetlc.color = grey;    
                }
                defensetlc.text = pbsdefenseStr + " / " + std::to_string(maxStats[classname][DEFENSE]);
                defenseDisplayStat.lastDisplayed = pbsdefenseStr;
                defensetlc.spawnframe = true; // so renderTextSystem can fix size
            }
            
            std::string pbsspeedStr = std::to_string(pbs.speed);
            if(speedDisplayStat.lastDisplayed != pbsspeedStr){
                if(pbs.speed == maxStats[classname][SPEED]){
                    speedtlc.color = maxstatcolor; 
                } else {
                    speedtlc.color = grey;    
                }
                speedtlc.text = pbsspeedStr + " / " + std::to_string(maxStats[classname][SPEED]);
                speedDisplayStat.lastDisplayed = pbsspeedStr;
                speedtlc.spawnframe = true; // so renderTextSystem can fix size
            }

            std::string pbsdexterityStr = std::to_string(pbs.dexterity);
            if(dexterityDisplayStat.lastDisplayed != pbsdexterityStr){
                if(pbs.dexterity == maxStats[classname][DEXTERITY]){
                    dexteritytlc.color = maxstatcolor; 
                } else {
                    dexteritytlc.color = grey;    
                }
                dexteritytlc.text = pbsdexterityStr + " / " + std::to_string(maxStats[classname][DEXTERITY]);
                dexterityDisplayStat.lastDisplayed = pbsdexterityStr;
                dexteritytlc.spawnframe = true; // so renderTextSystem can fix size
            }

            std::string pbswisdomStr = std::to_string(pbs.wisdom);
            if(wisdomDisplayStat.lastDisplayed != pbswisdomStr){
                if(pbs.wisdom == maxStats[classname][WISDOM]){
                    wisdomtlc.color = maxstatcolor; 
                } else {
                    wisdomtlc.color = grey;    
                }
                wisdomtlc.text = pbswisdomStr + " / " + std::to_string(maxStats[classname][WISDOM]);
                wisdomDisplayStat.lastDisplayed = pbswisdomStr;
                wisdomtlc.spawnframe = true; // so renderTextSystem can fix size
            }

            std::string pbsvitalityStr = std::to_string(pbs.vitality);
            if(vitalityDisplayStat.lastDisplayed != pbsvitalityStr){
                if(pbs.vitality == maxStats[classname][VITALITY]){
                    vitalitytlc.color = maxstatcolor; 
                } else {
                    vitalitytlc.color = grey;    
                }
                vitalitytlc.text = pbsvitalityStr + " / " + std::to_string(maxStats[classname][VITALITY]);
                vitalityDisplayStat.lastDisplayed = pbsvitalityStr;
                vitalitytlc.spawnframe = true; // so renderTextSystem can fix size
            }
        }

    } else { 
        hptext.invisible = mptext.invisible = xptext.invisible = true;
        if(wasDisplayingPotsToMaxLastFrame){
            wasDisplayingPotsToMaxLastFrame = false;
            eventBus->EmitEvent<UpdateDisplayStatEvent>(player);

            // must set att/def/... text.spawnFrame back to true so their sizes may be re-queried in renderTextSystem
            attacktlc.spawnframe = true;
        }
    }

}