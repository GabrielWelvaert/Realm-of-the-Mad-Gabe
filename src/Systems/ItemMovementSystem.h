#pragma once

#include "../ECS/ECS.h"
#include "../../libs/SDL2/SDL.h"
#include <vector>
#include "../Utils/tables.h"
#include "../Utils/enums.h"
#include "../Components/LootBagComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/InteractUIComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/LootBagCollisionEvent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/MouseBoxComponent.h"
#include "../AssetStore/AssetStore.h"
#include "../Components/PlayerItemsComponent.h"
#include "../Events/EquipItemWithStatsEvent.h"

/*
This system is responsible for allowing the player to move items between loot bags, their inventory, and their equipment slots
*/

class ItemMovementSystem: public System{
    private:                                    // weapon, ability, armor, ring
        std::vector<glm::vec2> equipPositions = {{773.5,458.5}, {830.5,458.5}, {886.5,458.5}, {942.5,458.5}};

        // poorly organized function but it works 
        inline void swapOrMoveItemPositions(const unsigned int& item1pos, std::map<unsigned char, Entity>& contents1, const glm::vec2& item1OriginalTransformPos, const unsigned int& destPos, std::map<unsigned char, Entity>& destBag, const glm::vec2& destTransformPos, std::unique_ptr<EventBus>& eventBus, std::unique_ptr<AssetStore>& assetStore, Entity player){
            if(contents1 == destBag && item1pos == destPos) { // same spot and bag, return
                auto& item1 = contents1.at(item1pos);
                item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
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
                        } else {
                            item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                            assetStore->PlaySound(ERROR);
                            return;
                        }
                    } else if(item1OriginalTransformPos == equipPositions[1]){ // grabbed item from ability slot 

                    } else if(item1OriginalTransformPos == equipPositions[2]){ // grabbed item from armor slot
                        std::cout << "item swap attempt" << std::endl;
                        if(itemToGroup.at(item2ic.itemEnum) == validarmor.at(classname)){
                            eventBus->EmitEvent<EquipItemWithStatsEvent>(true, item1ic.itemEnum, true, item2ic.itemEnum, player);
                            eventBus->EmitEvent<UpdateDisplayStatEvent>(player);
                        } else {
                            item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                            assetStore->PlaySound(ERROR);
                            return;
                        }
                    } else if(item1OriginalTransformPos == equipPositions[3]){ // grabbed item from ring alot
                        if(itemToGroup.at(item2ic.itemEnum) == RING){
                            eventBus->EmitEvent<EquipItemWithStatsEvent>(true, item1ic.itemEnum, true, item2ic.itemEnum, player);
                            eventBus->EmitEvent<UpdateDisplayStatEvent>(player);
                        } else {
                            item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                            assetStore->PlaySound(ERROR);
                            return;
                        }
                    }
                /*item is actually dropped in (two items involved)*/
                } else { // grabbed item not from equipment slot; item1 is item to equip
                    if(destTransformPos == equipPositions[0]){ // dropping item into occupied weapon slot
                        if(itemToGroup.at(item1ic.itemEnum) == validWeapons.at(classname)){
                            eventBus->EmitEvent<WeaponEquipEvent>(item1ic.itemEnum, player);
                        } else {
                            item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                            assetStore->PlaySound(ERROR);
                            return;
                        }
                    } else if(destTransformPos == equipPositions[1]){ // dropping item into occupied ability slot 

                    } else if(destTransformPos == equipPositions[2]){ // dropping item into occupied armor slot 
                        std::cout << "item drop attempt (occupied)" << std::endl;
                        if(itemToGroup.at(item1ic.itemEnum) == validarmor.at(classname)){
                            eventBus->EmitEvent<EquipItemWithStatsEvent>(true, item2ic.itemEnum, true, item1ic.itemEnum, player);
                            eventBus->EmitEvent<UpdateDisplayStatEvent>(player);
                        } else {
                            item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                            assetStore->PlaySound(ERROR);
                            return;
                        }

                    } else if(destTransformPos == equipPositions[3]){ // dropping item into occupied ring slot
                        if(itemToGroup.at(item1ic.itemEnum) == RING){
                            eventBus->EmitEvent<EquipItemWithStatsEvent>(true, item2ic.itemEnum, true, item1ic.itemEnum, player);
                            eventBus->EmitEvent<UpdateDisplayStatEvent>(player);
                        } else {
                            item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                            assetStore->PlaySound(ERROR);
                            return;
                        }
                    }
                }
                item1TransformPosition = item2TranformPosition;
                item2TranformPosition = item1OriginalTransformPos;
                item1ic.lastPosition = destPos;
                item2ic.lastPosition = item1pos;
                item1ic.hostMap = &destBag;
                item2ic.hostMap = &contents1;
                contents1[item1pos] = destBag[destPos];
                destBag[destPos] = tempitem.second;
                assetStore->PlaySound(INVENTORY); 
            /*one item involved*/
            } else { // destination spot is vacant; move 
                auto& item1 = contents1.at(item1pos);
                auto& ic = item1.GetComponent<ItemComponent>();
                if(destTransformPos == equipPositions[0]){ // dropping item into vacant weapon slot
                    if(itemToGroup.at(ic.itemEnum) == validWeapons.at(classname)){
                        eventBus->EmitEvent<WeaponEquipEvent>(ic.itemEnum, player);
                    } else {
                        item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                        assetStore->PlaySound(ERROR);
                        return;
                    }
                } else if(destTransformPos == equipPositions[1]){ // dropping item into vacant ability slot 

                } else if(destTransformPos == equipPositions[2]){ // dropping item into vacant armor slot 
                    if(itemToGroup.at(ic.itemEnum) == validarmor.at(classname)){
                        std::cout << "item drop attempt (not occupied)" << std::endl;
                        eventBus->EmitEvent<EquipItemWithStatsEvent>(false, ic.itemEnum, true, ic.itemEnum, player);
                        eventBus->EmitEvent<UpdateDisplayStatEvent>(player);
                    } else {
                        item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                        assetStore->PlaySound(ERROR);
                        return;
                    }

                } else if(destTransformPos == equipPositions[3]){ // dropping item into vacant ring slot
                    if(itemToGroup.at(ic.itemEnum) == RING){
                        eventBus->EmitEvent<EquipItemWithStatsEvent>(false, ic.itemEnum, true, ic.itemEnum, player);
                        eventBus->EmitEvent<UpdateDisplayStatEvent>(player);
                    } else {
                        item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                        assetStore->PlaySound(ERROR);
                        return;
                    }
                }
                if(item1OriginalTransformPos.y < 506 && (itemEnumToStatData.find(ic.itemEnum) != itemEnumToStatData.end())){ // equipped item was unequipped had stat component: must subtract stats
                    eventBus->EmitEvent<EquipItemWithStatsEvent>(true, ic.itemEnum, false, ic.itemEnum, player);
                    eventBus->EmitEvent<UpdateDisplayStatEvent>(player);
                }
                item1.GetComponent<TransformComponent>().position = destTransformPos;
                ic.lastPosition = destPos;
                ic.hostMap = &destBag;
                destBag[destPos] = contents1[item1pos];
                contents1.erase(item1pos);
                assetStore->PlaySound(INVENTORY); 
            }
    }
    
    public:

        ItemMovementSystem(){
            RequireComponent<MouseBoxComponent>();
            RequireComponent<ItemComponent>();
        }

        void Update(int mx, int my, bool clicking, std::unique_ptr<AssetStore>& assetStore, std::unique_ptr<Registry>& registry,std::unique_ptr<EventBus>& eventBus, Entity player){ // todo pass player inventory component and equipment component (use maps!)
            auto& playerInventory = player.GetComponent<PlayerItemsComponent>();
            const auto& classname = player.GetComponent<ClassNameComponent>().classname;
            for(const auto& entity: GetSystemEntities()){ // gets all visible items (only items have mouseBoxComponent)
                const auto& mb = entity.GetComponent<MouseBoxComponent>();
                auto& transform = entity.GetComponent<TransformComponent>();

                if(playerInventory.holdingItemLastFrame && playerInventory.IdOfHeldItem > 0){ // are we holding an item?
                    if(entity.GetId() != playerInventory.IdOfHeldItem){
                        continue; // continue until we get the held item! 
                    } else {
                        transform.position = {mx-mb.width/2,my-mb.height/2}; // held item follows mouse
                    }
                }

                //if mouse colliding with item, clicking, and not holding item last frame
                if(mx > transform.position.x && mx < transform.position.x + mb.width && my > transform.position.y && my < transform.position.y + mb.width && clicking && !playerInventory.holdingItemLastFrame){
                    playerInventory.holdingItemLastFrame = true;  
                    playerInventory.IdOfHeldItem = entity.GetId();
                    playerInventory.heldItemStartingTransformComp = transform.position;
                    // std::cout << "item in " << static_cast<int>(entity.GetComponent<ItemComponent>().lastPosition) << " positon grabbed... " << std::endl;
                    return;    
                } else if (playerInventory.holdingItemLastFrame && !clicking){
                    playerInventory.holdingItemLastFrame = playerInventory.IdOfHeldItem = 0;
                    if(mx > 988 || my > 743 || my < 447){
                        std::cout << "invalid item drop detected" << std::endl;
                        transform.position = playerInventory.heldItemStartingTransformComp;
                        assetStore->PlaySound(ERROR);
                        return; 
                    } 
                    auto& ic = entity.GetComponent<ItemComponent>();
                    if(mx < 750){ // TODO logic for if colliding with bag or not when dropping item
                        std::cout << "world-drop detected" << std::endl;
                        assetStore->PlaySound(LOOT);
                        return;
                    }
                    // "binary search" of possible item-drop locations
                    if(my > 627){ // loot bag
                        //todo: error-check if lootbag is open
                        if(!playerInventory.ptrToOpenBag){
                            transform.position = playerInventory.heldItemStartingTransformComp;
                            assetStore->PlaySound(ERROR);
                            return;
                        }
                        if(my > 686){ // loot bag bottom row
                            if(mx > 877){ // spot 7 or 8
                                if(mx > 934){ // spot 8
                                    std::cout << "spot 8 lootbag drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 8, *playerInventory.ptrToOpenBag, {942.5,696.5}, eventBus, assetStore, player);
                                    return;
                                } else { // spot 7
                                    std::cout << "spot 7 lootbag drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 7, *playerInventory.ptrToOpenBag, {886.5,696.5}, eventBus, assetStore, player);
                                    return;
                                }
                            } else { // spot 5 or 6
                                if(mx > 819){ // 6
                                    std::cout << "spot 6 lootbag drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 6, *playerInventory.ptrToOpenBag, {830.5,696.5}, eventBus, assetStore, player);
                                    return;
                                } else { // 5
                                    std::cout << "spot 5 lootbag drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 5, *playerInventory.ptrToOpenBag, {773.5,696.5}, eventBus, assetStore, player);
                                    return;
                                }
                            }
                        } else { // loot bag top row
                            if(mx > 877){ // 3 or 4
                                if(mx > 934){ // 4
                                    std::cout << "spot 4 lootbag drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 4, *playerInventory.ptrToOpenBag, {942.5,639.5}, eventBus, assetStore, player);
                                    return;
                                } else { // 3
                                    std::cout << "spot 3 lootbag drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 3, *playerInventory.ptrToOpenBag, {886.5,639.5}, eventBus, assetStore, player);
                                    return;
                                }
                            } else { //1 or 2 
                                if(mx>819){
                                    std::cout << "spot 2 lootbag drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 2, *playerInventory.ptrToOpenBag, {830.5,639.5}, eventBus, assetStore, player);
                                    return;
                                } else {
                                    std::cout << "spot 1 lootbag drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 1, *playerInventory.ptrToOpenBag, {773.5,639.5}, eventBus, assetStore, player);
                                    return;
                                }
                            }
                        }
                    } else { // above loot bag
                        if(my > 506){ // player inventory
                            if(my > 561){ // bottom row
                                if(mx > 877){ // 8 or 7
                                    if(mx > 934){ // 8
                                        std::cout << "spot 8 inventory drop" << std::endl;
                                        swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 8, playerInventory.inventory, {942.5,572.5}, eventBus, assetStore, player);
                                        return;
                                    } else { // 7
                                        std::cout << "spot 7 inventory drop" << std::endl;
                                        swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 7, playerInventory.inventory, {886.5,572.5}, eventBus, assetStore, player);
                                        return;
                                    }
                                } else { // 5 or 6
                                    if(mx > 819){ // 6
                                        std::cout << "spot 6 inventory drop" << std::endl;
                                        swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 6, playerInventory.inventory, {830.5,572.5}, eventBus, assetStore, player);
                                        return;
                                    } else { // 5
                                        std::cout << "spot 5 inventory drop" << std::endl;
                                        swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 5, playerInventory.inventory, {773.5,572.5}, eventBus, assetStore, player);
                                        return;
                                    }
                                }
                            } else { // top row
                                if(mx > 877){ // 3 or 4 
                                    if(mx > 934){ // 4
                                        std::cout << "spot 4 inventory drop" << std::endl;
                                        swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 4, playerInventory.inventory, {942.5,515.5}, eventBus, assetStore, player);
                                        return;
                                    } else { // 3
                                        std::cout << "spot 3 inventory drop" << std::endl;
                                        swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 3, playerInventory.inventory, {886.5,515.5}, eventBus, assetStore, player);
                                        return;
                                    }
                                } else {// 1 or 2
                                    if(mx > 819){ // 2
                                        std::cout << "spot 2 inventory drop" << std::endl;
                                        swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 2, playerInventory.inventory, {830.5,515.5}, eventBus, assetStore, player);
                                        return;
                                    } else { // 1
                                        std::cout << "spot 1 inventory drop" << std::endl;
                                        swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 1, playerInventory.inventory, {773.5,515.5}, eventBus, assetStore, player);
                                        return;
                                    }
                                }
                            }
                        } else { // equipment slots
                            if(mx > 877){
                                if(mx > 934){ // ring
                                    std::cout << "ring spot drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 4, playerInventory.equipment, {942.5,458.5}, eventBus, assetStore, player);
                                    return;
                                } else { // armor
                                    std::cout << "armor spot drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 3, playerInventory.equipment, {886.5,458.5}, eventBus, assetStore, player);
                                    return;
                                }
                            } else {
                                if(mx > 819){ // ability
                                    std::cout << "ability spot drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 2, playerInventory.equipment, {830.5,458.5}, eventBus, assetStore, player);
                                    return;
                                } else { // weapon
                                    std::cout << "weapon spot drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 1, playerInventory.equipment, {773.5,458.5}, eventBus, assetStore, player);
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }

};