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

/*
This system is responsible for allowing the player to move items between loot bags, their inventory, and their equipment slots
*/

class ItemMovementSystem: public System{
    private:
        // parameters: position of item 1, host std::map of item 1, transform of item 1 destination position of item, destination std::map of item, glm::vec2 for destination sprite position
        // last parameter is only needed in the case that there does not exist at item in the destPos to see its transform.position from!
        // if destination does not contain item, will move instead of swap
        inline void swapOrMoveItemPositions(const unsigned int& item1pos, std::map<unsigned char, Entity>& contents1, const glm::vec2& item1OriginalTransformPos, const unsigned int& destPos, std::map<unsigned char, Entity>& destBag, const glm::vec2& destTransformPos){
            if(contents1 == destBag && item1pos == destPos) {
                // std::cout << "same spot and bag case detected" << std::endl;
                auto& item1 = contents1.at(item1pos);
                item1.GetComponent<TransformComponent>().position = item1OriginalTransformPos;
                return;
            } // same spot and bag, return
            if(destBag.find(destPos) != destBag.end()){ // item found in desintation spot; swap
                std::cout << "attempting item swap" << std::endl;
                std::pair<unsigned char, Entity> tempitem = *contents1.find(item1pos);
                auto& item1 = contents1.at(item1pos);
                auto& item2 = destBag.at(destPos);
                auto& item1TransformPosition = item1.GetComponent<TransformComponent>().position;
                auto& item2TranformPosition = item2.GetComponent<TransformComponent>().position;
                item1TransformPosition = item2TranformPosition;
                item2TranformPosition = item1OriginalTransformPos;
                auto& item1ic = item1.GetComponent<ItemComponent>();
                auto& item2ic = item2.GetComponent<ItemComponent>();
                item1ic.lastPosition = destPos;
                item2ic.lastPosition = item1pos;
                item1ic.hostMap = &destBag;
                item2ic.hostMap = &contents1;
                contents1[item1pos] = destBag[destPos];
                destBag[destPos] = tempitem.second;
            } else { // destination spot is vacant; move 
                auto& item1 = contents1.at(item1pos);
                item1.GetComponent<TransformComponent>().position = destTransformPos;
                item1.GetComponent<ItemComponent>().lastPosition = destPos;
                item1.GetComponent<ItemComponent>().hostMap = &destBag;
                destBag[destPos] = contents1[item1pos];
                contents1.erase(item1pos);
            }
    }
    
    public:

        ItemMovementSystem(){
            RequireComponent<MouseBoxComponent>();
            RequireComponent<ItemComponent>();
        }

        void Update(int mx, int my, bool clicking, std::unique_ptr<AssetStore>& assetStore, std::unique_ptr<Registry>& registry, PlayerItemsComponent& playerInventory){ // todo pass player inventory component and equipment component (use maps!)
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
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 8, *playerInventory.ptrToOpenBag, {942.5,696.5});
                                    assetStore->PlaySound(INVENTORY);
                                    return;
                                } else { // spot 7
                                    std::cout << "spot 7 lootbag drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 7, *playerInventory.ptrToOpenBag, {886.5,696.5});
                                    assetStore->PlaySound(INVENTORY);
                                    return;
                                }
                            } else { // spot 5 or 6
                                if(mx > 819){ // 6
                                    std::cout << "spot 6 lootbag drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 6, *playerInventory.ptrToOpenBag, {830.5,696.5});
                                    assetStore->PlaySound(INVENTORY);
                                    return;
                                } else { // 5
                                    std::cout << "spot 5 lootbag drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 5, *playerInventory.ptrToOpenBag, {773.5,696.5});
                                    assetStore->PlaySound(INVENTORY);
                                    return;
                                }
                            }
                        } else { // loot bag top row
                            if(mx > 877){ // 3 or 4
                                if(mx > 934){ // 4
                                    std::cout << "spot 4 lootbag drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 4, *playerInventory.ptrToOpenBag, {942.5,639.5});
                                    assetStore->PlaySound(INVENTORY);
                                    return;
                                } else { // 3
                                    std::cout << "spot 3 lootbag drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 3, *playerInventory.ptrToOpenBag, {886.5,639.5});
                                    assetStore->PlaySound(INVENTORY);
                                    return;
                                }
                            } else { //1 or 2 
                                if(mx>819){
                                    std::cout << "spot 2 lootbag drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 2, *playerInventory.ptrToOpenBag, {830.5,639.5});
                                    assetStore->PlaySound(INVENTORY);
                                    return;
                                } else {
                                    std::cout << "spot 1 lootbag drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 1, *playerInventory.ptrToOpenBag, {773.5,639.5});
                                    assetStore->PlaySound(INVENTORY);
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
                                        swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 8, playerInventory.inventory, {942.5,572.5});
                                        assetStore->PlaySound(INVENTORY);
                                        return;
                                    } else { // 7
                                        std::cout << "spot 7 inventory drop" << std::endl;
                                        swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 7, playerInventory.inventory, {886.5,572.5});
                                        assetStore->PlaySound(INVENTORY);
                                        return;
                                    }
                                } else { // 5 or 6
                                    if(mx > 819){ // 6
                                        std::cout << "spot 6 inventory drop" << std::endl;
                                        swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 6, playerInventory.inventory, {830.5,572.5});
                                        assetStore->PlaySound(INVENTORY);
                                        return;
                                    } else { // 5
                                        std::cout << "spot 5 inventory drop" << std::endl;
                                        swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 5, playerInventory.inventory, {773.5,572.5});
                                        assetStore->PlaySound(INVENTORY);
                                        return;
                                    }
                                }
                            } else { // top row
                                if(mx > 877){ // 3 or 4 
                                    if(mx > 934){ // 4
                                        std::cout << "spot 4 inventory drop" << std::endl;
                                        swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 4, playerInventory.inventory, {942.5,515.5});
                                        assetStore->PlaySound(INVENTORY);
                                        return;
                                    } else { // 3
                                        std::cout << "spot 3 inventory drop" << std::endl;
                                        swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 3, playerInventory.inventory, {886.5,515.5});
                                        assetStore->PlaySound(INVENTORY);
                                        return;
                                    }
                                } else {// 1 or 2
                                    if(mx > 819){ // 2
                                        std::cout << "spot 2 inventory drop" << std::endl;
                                        swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 2, playerInventory.inventory, {830.5,515.5});
                                        assetStore->PlaySound(INVENTORY);
                                        return;
                                    } else { // 1
                                        std::cout << "spot 1 inventory drop" << std::endl;
                                        swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 1, playerInventory.inventory, {773.5,515.5});
                                        assetStore->PlaySound(INVENTORY);
                                        return;
                                    }
                                }
                            }
                        } else { // equipment slots
                            if(mx > 877){
                                if(mx > 934){ // ring
                                    std::cout << "ring spot drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 4, playerInventory.equipment, {942.5,458.5});
                                    assetStore->PlaySound(INVENTORY);
                                    return;
                                } else { // armor
                                    std::cout << "armor spot drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 3, playerInventory.equipment, {886.5,458.5});
                                    assetStore->PlaySound(INVENTORY);
                                    return;
                                }
                            } else {
                                if(mx > 819){ // ability
                                    std::cout << "ability spot drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 2, playerInventory.equipment, {830.5,458.5});
                                    assetStore->PlaySound(INVENTORY);
                                    return;
                                } else { // weapon
                                    std::cout << "weapon spot drop" << std::endl;
                                    swapOrMoveItemPositions(ic.lastPosition, *ic.hostMap, playerInventory.heldItemStartingTransformComp, 1, playerInventory.equipment, {773.5,458.5});
                                    assetStore->PlaySound(INVENTORY);
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }

};