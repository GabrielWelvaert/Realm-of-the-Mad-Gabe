#ifndef ITEMMOVEMENTSYSTEM_H
#define ITEMMOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../../libs/SDL2/SDL.h"
#include <vector>
#include "../Utils/tables.h"
#include "../Utils/enums.h"
#include "../Components/LootBagComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/InteractUIComponent.h"
#include "../Components/ItemIconComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/LootBagCollisionEvent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/MouseBoxComponent.h"
#include "../AssetStore/AssetStore.h"
#include "../Components/PlayerItemsComponent.h"
#include "../Events/EquipItemWithStatsEvent.h"
#include "../Components/AbilityComponent.h"
#include "../Events/EquipAbilityEvent.h"
#include "../AssetStore/AssetStore.h"
#include "../Events/DrinkConsumableEvent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Events/UpdateDisplayStatEvent.h"
#include "../Components/ClassNameComponent.h"
#include "../Utils/factory.h"
#include "../Events/WeaponEquipEvent.h"
#include "../Events/KillItemIconEvent.h"
#include "../Components/ItemComponent.h"

/*
This system is responsible for allowing the player to move items between loot bags, their inventory, and their equipment slots
This system should not be marveled for its organization but only simply if you want to for its functionality; it has a plethora of repeated logic.
It is extremely hard to read, debug, and develop. This is due to requirements developing alongside the system... truly.. spaghetti!
*/

class ItemMovementSystem: public System{
    private:                                    // weapon, ability, armor, ring
        std::vector<glm::vec2> equipPositions = {{773.5,458.5}, {830.5,458.5}, {886.5,458.5}, {942.5,458.5}};
        std::vector<glm::vec2> lootbagPositions = {{773.5, 639.5}, {830.5, 639.5}, {886.5, 639.5}, {942.5, 639.5}, {773.5, 696.5}, {830.5, 696.5}, {886.5, 696.5}, {942.5, 696.5}};
        std::vector<glm::vec2> inventoryPositions = {{773.5, 515.5}, {830.5, 515.5}, {886.5, 515.5}, {942.5, 515.5}, {773.5, 572.5}, {830.5, 572.5}, {886.5, 572.5}, {942.5, 572.5}};
        // bool shiftblock = false;
        std::map<unsigned char, Entity> * originbag;

        inline void hideIcon(std::unique_ptr<Registry>& registry, int id){
            registry->GetComponent<SpriteComponent>(id).zIndex = 9;
        }

        inline void showIcon(std::unique_ptr<Registry>& registry, int id){
            registry->GetComponent<SpriteComponent>(id).zIndex = 11;
        }

        // poorly organized and overcomplicated function but it works 
        inline void swapOrMoveItemPositions(const unsigned int& item1pos, 
            std::map<unsigned char, Entity>& contents1, 
            const glm::vec2& item1OriginalTransformPos, 
            const unsigned int& destPos, 
            std::map<unsigned char, 
            Entity>& destBag, 
            const glm::vec2& destTransformPos, 
            std::unique_ptr<EventBus>& eventBus, 
            std::unique_ptr<AssetStore>& assetStore, 
            Entity player, 
            std::unique_ptr<Registry>& registry, 
            const std::vector<int>& inventoryIcons, 
            const std::vector<int>& equipmentIcons, 
            const bool& shift)
            {
            // std::cout << "spawOrMoveItemPositions called with an item from bag with original position " << item1OriginalTransformPos.x << ", " << item1OriginalTransformPos.y << std::endl;
            // std::cout << item1pos << " " << destPos << std::endl;
            if(contents1 == destBag && item1pos == destPos) { // same spot and bag, return
                auto& item1 = contents1.at(item1pos);
                item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                
                if(item1OriginalTransformPos.y > 506){ // item from inventory

                } else { // item from equipment slot
                    hideIcon(registry, equipmentIcons[item1.GetComponent<ItemComponent>().lastPosition-1]);
                }
                     
                return;
            } 
            const auto& classname = player.GetComponent<ClassNameComponent>().classname;
            /*two items involved*/
            if(destBag.find(destPos) != destBag.end()){ // item found in desintation spot; swap
                // std::cout << "attempting item swap" << std::endl;
                std::pair<unsigned char, Entity> tempitem = *contents1.find(item1pos);
                auto& item1 = contents1.at(item1pos);
                auto& item2 = destBag.at(destPos);
                auto& item1TransformPosition = item1.GetComponent<TransformComponent>().position;
                auto& item2TranformPosition = item2.GetComponent<TransformComponent>().position;
                auto& item1ic = item1.GetComponent<ItemComponent>();
                auto& item2ic = item2.GetComponent<ItemComponent>();
                /*case where item is swapped in rather than dropped it (two items involved)*/
                if(item1OriginalTransformPos.y < 506){ // grabbed item is from equipment slot; item2 is item to equip 
                    if(item1OriginalTransformPos == equipPositions[0]){ // grabbed item from weapon slot
                        if(itemToGroup.at(item2ic.itemEnum) == validWeapons.at(classname)){
                            eventBus->EmitEvent<WeaponEquipEvent>(item2ic.itemEnum, player);
                            hideIcon(registry, equipmentIcons[0]);
                        } else {
                            item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                            assetStore->PlaySound(ERROR);
                            hideIcon(registry, equipmentIcons[0]);
                            return;
                        }
                    } else if(item1OriginalTransformPos == equipPositions[1]){ // grabbed item from ability slot 
                        if(itemToGroup.at(item2ic.itemEnum) == validability.at(classname)){
                            eventBus->EmitEvent<EquipAbilityEvent>(player, item2ic.itemEnum);
                            if(itemEnumToStatData.find(item1ic.itemEnum) != itemEnumToStatData.end()){
                                eventBus->EmitEvent<EquipItemWithStatsEvent>(true, item1ic.itemEnum, true, item2ic.itemEnum, player);
                                eventBus->EmitEvent<UpdateDisplayStatEvent>(player);
                                hideIcon(registry, equipmentIcons[1]);
                            }
                        } else {
                            item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                            assetStore->PlaySound(ERROR);
                            hideIcon(registry, equipmentIcons[1]);
                            return;
                        }
                    } else if(item1OriginalTransformPos == equipPositions[2]){ // grabbed item from armor slot
                        if(itemToGroup.at(item2ic.itemEnum) == validarmor.at(classname)){
                            eventBus->EmitEvent<EquipItemWithStatsEvent>(true, item1ic.itemEnum, true, item2ic.itemEnum, player);
                            eventBus->EmitEvent<UpdateDisplayStatEvent>(player);
                            hideIcon(registry, equipmentIcons[2]);
                        } else {
                            item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                            assetStore->PlaySound(ERROR);
                            hideIcon(registry, equipmentIcons[2]);
                            return;
                        }
                    } else if(item1OriginalTransformPos == equipPositions[3]){ // grabbed item from ring alot
                        if(itemToGroup.at(item2ic.itemEnum) == RING){
                            eventBus->EmitEvent<EquipItemWithStatsEvent>(true, item1ic.itemEnum, true, item2ic.itemEnum, player);
                            eventBus->EmitEvent<UpdateDisplayStatEvent>(player);
                            hideIcon(registry, equipmentIcons[3]);
                        } else {
                            item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                            assetStore->PlaySound(ERROR);
                            hideIcon(registry, equipmentIcons[3]);
                            return;
                        }
                    }
                /*item is actually dropped in (two items involved)*/
                } else { // grabbed item not from equipment slot; item1 is item to equip
                    if(destTransformPos == equipPositions[0]){ // dropping item into occupied weapon slot
                        if(itemToGroup.at(item1ic.itemEnum) == validWeapons.at(classname)){
                            eventBus->EmitEvent<WeaponEquipEvent>(item1ic.itemEnum, player);
                            hideIcon(registry, equipmentIcons[0]);
                        } else {
                            item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                            assetStore->PlaySound(ERROR);
                            hideIcon(registry, equipmentIcons[0]);
                            return;
                        }
                    } else if(destTransformPos == equipPositions[1]){ // dropping item into occupied ability slot 
                        if(itemToGroup.at(item1ic.itemEnum) == validability.at(classname)){
                            eventBus->EmitEvent<EquipAbilityEvent>(player, item1ic.itemEnum);
                            if(itemEnumToStatData.find(item2ic.itemEnum) != itemEnumToStatData.end()){
                                eventBus->EmitEvent<EquipItemWithStatsEvent>(true, item2ic.itemEnum, true, item1ic.itemEnum, player);
                                eventBus->EmitEvent<UpdateDisplayStatEvent>(player);
                            }
                            hideIcon(registry, equipmentIcons[1]);
                        } else {
                            item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                            assetStore->PlaySound(ERROR);
                            hideIcon(registry, equipmentIcons[1]);
                            return;
                        }
                    } else if(destTransformPos == equipPositions[2]){ // dropping item into occupied armor slot 
                        if(itemToGroup.at(item1ic.itemEnum) == validarmor.at(classname)){
                            eventBus->EmitEvent<EquipItemWithStatsEvent>(true, item2ic.itemEnum, true, item1ic.itemEnum, player);
                            eventBus->EmitEvent<UpdateDisplayStatEvent>(player);
                            hideIcon(registry, equipmentIcons[2]);
                        } else {
                            item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                            assetStore->PlaySound(ERROR);
                            hideIcon(registry, equipmentIcons[2]);
                            return;
                        }

                    } else if(destTransformPos == equipPositions[3]){ // dropping item into occupied ring slot
                        if(itemToGroup.at(item1ic.itemEnum) == RING){
                            eventBus->EmitEvent<EquipItemWithStatsEvent>(true, item2ic.itemEnum, true, item1ic.itemEnum, player);
                            eventBus->EmitEvent<UpdateDisplayStatEvent>(player);
                            hideIcon(registry, equipmentIcons[3]);
                        } else {
                            item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                            assetStore->PlaySound(ERROR);
                            hideIcon(registry, equipmentIcons[3]);
                            return;
                        }
                    }
                }
                item1TransformPosition = item2TranformPosition;
                item2TranformPosition = item1OriginalTransformPos;
                item1ic.lastPosition = destPos;
                item2ic.lastPosition = item1pos;
                contents1[item1pos] = destBag[destPos];
                destBag[destPos] = tempitem.second;
                assetStore->PlaySound(INVENTORY); 
                // if(item1TransformPosition == item2TranformPosition){
                //     std::cout << "broken shit detected use gdb bt" << std::endl;
                // }
            /*one item involved*/
            } else { // destination spot is vacant; move 
                auto& item1 = contents1.at(item1pos);
                auto& ic = item1.GetComponent<ItemComponent>();
                if(destTransformPos == equipPositions[0]){ // dropping item into vacant weapon slot
                    if(itemToGroup.at(ic.itemEnum) == validWeapons.at(classname)){ 
                        hideIcon(registry, equipmentIcons[0]);
                        eventBus->EmitEvent<WeaponEquipEvent>(ic.itemEnum, player);
                    } else {
                        item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                        assetStore->PlaySound(ERROR);
                        return;
                    }
                } else if(destTransformPos == equipPositions[1]){ // dropping item into vacant ability slot 
                    if(itemToGroup.at(ic.itemEnum) == validability.at(classname)){
                        hideIcon(registry, equipmentIcons[1]);
                        eventBus->EmitEvent<EquipAbilityEvent>(player, ic.itemEnum);
                        if(itemEnumToStatData.find(ic.itemEnum) != itemEnumToStatData.end()){
                            eventBus->EmitEvent<EquipItemWithStatsEvent>(false, ic.itemEnum, true, ic.itemEnum, player);
                            eventBus->EmitEvent<UpdateDisplayStatEvent>(player);
                        }
                    } else{
                        item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                        assetStore->PlaySound(ERROR);
                        return;
                    }
                } else if(destTransformPos == equipPositions[2]){ // dropping item into vacant armor slot 
                    if(itemToGroup.at(ic.itemEnum) == validarmor.at(classname)){
                        hideIcon(registry, equipmentIcons[2]);
                        eventBus->EmitEvent<EquipItemWithStatsEvent>(false, ic.itemEnum, true, ic.itemEnum, player);
                        eventBus->EmitEvent<UpdateDisplayStatEvent>(player);
                    } else {
                        item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                        assetStore->PlaySound(ERROR);
                        return;
                    }

                } else if(destTransformPos == equipPositions[3]){ // dropping item into vacant ring slot
                    if(itemToGroup.at(ic.itemEnum) == RING){
                        hideIcon(registry, equipmentIcons[3]);
                        eventBus->EmitEvent<EquipItemWithStatsEvent>(false, ic.itemEnum, true, ic.itemEnum, player);
                        eventBus->EmitEvent<UpdateDisplayStatEvent>(player);
                    } else {
                        item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                        assetStore->PlaySound(ERROR);
                        return;
                    }
                } else if(destTransformPos.y > 506 && destTransformPos.y < 627){
                }
                if(item1OriginalTransformPos.y < 506){ // equipped item was unequipped
                    // std::cout << "equipped item unequipped detected (no swap) " << std::endl;
                    if(item1OriginalTransformPos == equipPositions[0]){ // weapon was unequipped; no longer holding weapon!
                        player.GetComponent<ProjectileEmitterComponent>().shots = 0;
                        showIcon(registry, equipmentIcons[0]);
                    } else if (item1OriginalTransformPos == equipPositions[1]){
                        showIcon(registry, equipmentIcons[1]);
                        player.GetComponent<AbilityComponent>().abilityEquipped = false;
                    } else if(item1OriginalTransformPos == equipPositions[2]){ // restore armor slot icon
                        showIcon(registry, equipmentIcons[2]);
                    } else if(item1OriginalTransformPos == equipPositions[3]){ // restore ring slot icon
                        showIcon(registry, equipmentIcons[3]);
                    }
                    if(itemEnumToStatData.find(ic.itemEnum) != itemEnumToStatData.end()){ // unequipped item had stats; subtract those stats
                        eventBus->EmitEvent<EquipItemWithStatsEvent>(true, ic.itemEnum, false, ic.itemEnum, player);
                        eventBus->EmitEvent<UpdateDisplayStatEvent>(player);
                    }
                } else if (item1OriginalTransformPos.y < 627){ // item originated in inventory
                } 
                item1.GetComponent<TransformComponent>().position = destTransformPos;
                ic.lastPosition = destPos;
                destBag[destPos] = contents1[item1pos];
                contents1.erase(item1pos);
                assetStore->PlaySound(INVENTORY); 
            }
    }
    
    public:

        ItemMovementSystem();
        // place item into inventory slot of player. Only used once when spawning player; never use otherwise. assumes vacancy in position 
        inline void ForcePlayerEquipItem(std::unique_ptr<Registry>& registry, std::unique_ptr<EventBus>& eventbus, Entity& player, const items& itemEnum, const int& position, const std::vector<int>& equipmentIconIds){
            Entity item = registry->CreateEntity();
            auto& equipment = player.GetComponent<PlayerItemsComponent>().equipment;
            auto& classname = player.GetComponent<ClassNameComponent>().classname;
            item.AddComponent<SpriteComponent>(itemEnum);
            item.AddComponent<ItemComponent>(itemEnum, position);
            item.AddComponent<MouseBoxComponent>(40,40);
            item.Group(itemToGroup.at(itemEnum));
            switch(position){
                case 1:{
                    if(itemToGroup.at(itemEnum) == validWeapons.at(classname)){
                        equipment[position] = item;
                        eventbus->EmitEvent<WeaponEquipEvent>(itemEnum, player);
                        item.AddComponent<TransformComponent>(equipPositions[0], glm::vec2(5.0,5.0));
                        hideIcon(registry, equipmentIconIds[0]);
                        registry->Update();
                    } else {
                        return;
                    }
                }break;
                case 2:{
                    if(itemToGroup.at(itemEnum) == validability.at(classname)){
                        equipment[position] = item;
                        eventbus->EmitEvent<EquipAbilityEvent>(player, itemEnum);
                        item.AddComponent<TransformComponent>(equipPositions[1], glm::vec2(5.0,5.0));
                        if(itemEnumToStatData.find(itemEnum) != itemEnumToStatData.end()){
                            eventbus->EmitEvent<EquipItemWithStatsEvent>(false, itemEnum, true, itemEnum, player);
                            eventbus->EmitEvent<UpdateDisplayStatEvent>(player);
                        }
                        hideIcon(registry, equipmentIconIds[1]);
                        registry->Update();
                    } else {
                        return;
                    }
                }break;
                case 3:{
                    if(itemToGroup.at(itemEnum) == validarmor.at(classname)){
                        equipment[position] = item;
                        eventbus->EmitEvent<EquipItemWithStatsEvent>(false, itemEnum, true, itemEnum, player);
                        eventbus->EmitEvent<UpdateDisplayStatEvent>(player);
                        item.AddComponent<TransformComponent>(equipPositions[2], glm::vec2(5.0,5.0));
                        hideIcon(registry, equipmentIconIds[2]);
                        registry->Update();
                    } else {
                        return;
                    }
                }break;
                case 4:{
                    if(itemToGroup.at(itemEnum) == RING){
                        equipment[position] = item;
                        eventbus->EmitEvent<EquipItemWithStatsEvent>(false, itemEnum, true, itemEnum, player);
                        eventbus->EmitEvent<UpdateDisplayStatEvent>(player);
                        item.AddComponent<TransformComponent>(equipPositions[3], glm::vec2(5.0,5.0));
                        hideIcon(registry, equipmentIconIds[3]);
                        registry->Update();
                    } else {
                        return;
                    }
                }break;
            }
        }   

        // place item into equipment slot of player and equip it. Only used once when spawning player; never use otherwise. assumes vacancy in position 
        inline void ForcePlayerPopulateInventory(std::unique_ptr<Registry>& registry, Entity& player, const items& itemEnum, const int& position){
            auto& inventory = player.GetComponent<PlayerItemsComponent>().inventory;
            Entity item = registry->CreateEntity();
            // std::cout << "spawning " << itemToName.at(itemEnum) << " w/ id " << item.GetId() << '\n'; 
            item.AddComponent<SpriteComponent>(itemEnum);
            item.AddComponent<ItemComponent>(itemEnum, position);
            item.AddComponent<MouseBoxComponent>(40,40);
            item.AddComponent<TransformComponent>(inventoryPositions[position-1], glm::vec2(5.0,5.0));
            item.Group(itemToGroup.at(itemEnum));
            inventory[position] = item;
        }

        void Update(int mx, int my, bool clicking, std::unique_ptr<AssetStore>& assetStore, std::unique_ptr<Registry>& registry,std::unique_ptr<EventBus>& eventBus, Entity player, const std::vector<int>& inventoryIcons, const std::vector<int>& equipmentIcons, std::unique_ptr<Factory>& factory, const bool& shift);

};

#endif