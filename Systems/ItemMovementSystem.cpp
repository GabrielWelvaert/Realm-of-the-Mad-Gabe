#include "ItemMovementSystem.h"

ItemMovementSystem::ItemMovementSystem(){
    RequireComponent<MouseBoxComponent>();
    RequireComponent<ItemComponent>();
}

void ItemMovementSystem::Update(int mx, int my, bool clicking, std::unique_ptr<AssetStore>& assetStore, std::unique_ptr<Registry>& registry,std::unique_ptr<EventBus>& eventBus, Entity player, const std::vector<int>& inventoryIcons, const std::vector<int>& equipmentIcons, std::unique_ptr<Factory>& factory, const bool& shift){ // todo pass player inventory component and equipment component (use maps!)
    auto& playerInventory = player.GetComponent<PlayerItemsComponent>();
    const auto& classname = player.GetComponent<ClassNameComponent>().classname;
    for(auto& entity: GetSystemEntities()){ // gets all visible items (only items have mouseBoxComponent)
        const auto& mb = entity.GetComponent<MouseBoxComponent>();
        auto& transform = entity.GetComponent<TransformComponent>();

        if(playerInventory.holdingItemLastFrame && playerInventory.IdOfHeldItem > 0){ // are we holding an item?
            if(entity.GetId() != playerInventory.IdOfHeldItem){
                continue; // continue until we get the held item! 
            } else {
                transform.position = {mx-mb.width/2,my-mb.height/2}; // held item follows mouse
            }
        }
        auto& sprite = entity.GetComponent<SpriteComponent>();

        if(!shift) {shiftblock=false;}

        //if mouse colliding with item, clicking, and not holding item last frame (logic for first frame of holding an item!)
        if(mx > transform.position.x && mx < transform.position.x + mb.width && my > transform.position.y && my < transform.position.y + mb.width && clicking && !playerInventory.holdingItemLastFrame){
            if(shift && my > 506 && !shiftblock){ // player shift-clicked consumable item from inventory or lootbag! 
                // some logic and flag to block shift until its released
                const auto& itemEnum = entity.GetComponent<ItemComponent>().itemEnum;
                if(static_cast<int>(itemToGroup.at(itemEnum)) >= 16){ // consumable item clicked from inventory or loot bag
                    shiftblock = false;
                    //binary search. wee
                    if(my < 627){ // inventory 
                        if(my < 561){ 
                            if(mx < 877){ 
                                if(mx < 819){ //1
                                    eventBus->EmitEvent<DrinkConsumableEvent>(player, itemEnum, registry, assetStore, eventBus, 1);
                                } else { //2
                                    eventBus->EmitEvent<DrinkConsumableEvent>(player, itemEnum, registry, assetStore, eventBus, 2);
                                }
                            } else { 
                                if(mx < 934){ // 3 
                                    eventBus->EmitEvent<DrinkConsumableEvent>(player, itemEnum, registry, assetStore, eventBus, 3);
                                } else { // 4
                                    eventBus->EmitEvent<DrinkConsumableEvent>(player, itemEnum, registry, assetStore, eventBus, 4);
                                }
                            }
                        } else { 
                            if(mx < 877){ 
                                if(mx < 819){ //5
                                    eventBus->EmitEvent<DrinkConsumableEvent>(player, itemEnum, registry, assetStore, eventBus, 5);
                                } else { //6
                                    eventBus->EmitEvent<DrinkConsumableEvent>(player, itemEnum, registry, assetStore, eventBus, 6);
                                }
                            } else { 
                                if(mx < 934){ // 7
                                    eventBus->EmitEvent<DrinkConsumableEvent>(player, itemEnum, registry, assetStore, eventBus, 7);
                                } else { // 8
                                    eventBus->EmitEvent<DrinkConsumableEvent>(player, itemEnum, registry, assetStore, eventBus, 8);
                                }
                            }
                        }
                    } else { // loot bag 
                            if(my < 686){ // 1-4
                            if(mx < 877){ 
                                if(mx < 819){ //1
                                    eventBus->EmitEvent<DrinkConsumableEvent>(player, itemEnum, registry, assetStore, eventBus, 1, false);
                                } else { //2
                                    eventBus->EmitEvent<DrinkConsumableEvent>(player, itemEnum, registry, assetStore, eventBus, 2, false);
                                }
                            } else { // 3-4
                                if(mx < 934){ // 3 
                                    eventBus->EmitEvent<DrinkConsumableEvent>(player, itemEnum, registry, assetStore, eventBus, 3, false);
                                } else { // 4
                                    eventBus->EmitEvent<DrinkConsumableEvent>(player, itemEnum, registry, assetStore, eventBus, 4, false);
                                }
                            }
                        } else { // 5-8
                            if(mx < 877){ // 56
                                if(mx < 819){ //5
                                    eventBus->EmitEvent<DrinkConsumableEvent>(player, itemEnum, registry, assetStore, eventBus, 5, false);
                                } else { //6
                                    eventBus->EmitEvent<DrinkConsumableEvent>(player, itemEnum, registry, assetStore, eventBus, 6, false);
                                }
                            } else { // 3-4
                                if(mx < 934){ // 7 
                                    eventBus->EmitEvent<DrinkConsumableEvent>(player, itemEnum, registry, assetStore, eventBus, 7, false);
                                } else { // 8
                                    eventBus->EmitEvent<DrinkConsumableEvent>(player, itemEnum, registry, assetStore, eventBus, 8, false);
                                }
                            }
                        }
                    }
                } else {
                    shiftblock = true;
                    assetStore->PlaySound(ERROR);
                }
                playerInventory.holdingItemLastFrame = playerInventory.IdOfHeldItem = 0;
                return;
            }
            playerInventory.holdingItemLastFrame = true;  
            playerInventory.IdOfHeldItem = entity.GetId();
            playerInventory.heldItemStartingTransformComp = transform.position;
            sprite.zIndex = 20;
            if(transform.position.y > 506){ // item from inventory

            } else { // item from equipment slot
                // std::cout << "here" << std::endl;
                showIcon(registry, equipmentIcons[entity.GetComponent<ItemComponent>().lastPosition-1]);
            }
                
            return;    
        } else if (playerInventory.holdingItemLastFrame && !clicking){ // item released
            playerInventory.holdingItemLastFrame = playerInventory.IdOfHeldItem = 0;
            sprite.zIndex = 12;
            if(mx > 750 && (mx > 988 || my > 743 || my < 447)){
                transform.position = playerInventory.heldItemStartingTransformComp;
                assetStore->PlaySound(ERROR);
                if(transform.position.y > 506){ // item from inventory

                } else { // item from equipment slot
                    hideIcon(registry, equipmentIcons[entity.GetComponent<ItemComponent>().lastPosition-1]);
                }
                return; 
            } 
            auto& ic = entity.GetComponent<ItemComponent>();
            if(mx < 750){ // TODO logic for if colliding with bag or not when dropping item
                if(playerInventory.heldItemStartingTransformComp.y > 627){
                    transform.position = playerInventory.heldItemStartingTransformComp;
                    assetStore->PlaySound(INVENTORY);
                    return;
                }
                unsigned char pos;
                if(playerInventory.viewingBag && registry->GetComponent<LootBagComponent>(playerInventory.IdOfOpenBag).contents.size() < 8){ //add to existing bag
                    pos = registry->GetComponent<LootBagComponent>(playerInventory.IdOfOpenBag).addItem(entity);
                    assetStore->PlaySound(INVENTORY);
                    entity.GetComponent<TransformComponent>().position = lootbagPositions.at(pos-1);
                } else { // spawn new lootbag if no existing bag or existing bag is full
                    Entity lootbag = factory->creatLootBag(registry, player.GetComponent<TransformComponent>().position, BROWNLOOTBAG);                            
                    pos = lootbag.GetComponent<LootBagComponent>().addItem(entity);
                    transform.position.x = -10000000; // couldn't remove and add transform/sprite... temporary solution ()
                    assetStore->PlaySound(LOOT);
                }
                if(playerInventory.heldItemStartingTransformComp.y < 506){ // item from equip
                    playerInventory.equipment.erase(entity.GetComponent<ItemComponent>().lastPosition);
                    // std::cout << "erasing item from equipment at pos " << static_cast<int>(entity.GetComponent<ItemComponent>().lastPosition) << std::endl;
                    showIcon(registry, equipmentIcons[entity.GetComponent<ItemComponent>().lastPosition-1]);
                    if(playerInventory.heldItemStartingTransformComp == equipPositions[0]){ // weapon was unequipped; no longer holding weapon!
                        player.GetComponent<ProjectileEmitterComponent>().shots = 0;
                        showIcon(registry, equipmentIcons[0]);
                    } else if (playerInventory.heldItemStartingTransformComp == equipPositions[1]){
                        showIcon(registry, equipmentIcons[1]);
                        player.GetComponent<AbilityComponent>().abilityEquipped = false;
                    } else if(playerInventory.heldItemStartingTransformComp == equipPositions[2]){ // restore armor slot icon
                        showIcon(registry, equipmentIcons[2]);
                    } else if(playerInventory.heldItemStartingTransformComp == equipPositions[3]){ // restore ring slot icon
                        showIcon(registry, equipmentIcons[3]);
                    }
                    if(itemEnumToStatData.find(ic.itemEnum) != itemEnumToStatData.end()){ // unequipped item had stats; subtract those stats
                        eventBus->EmitEvent<EquipItemWithStatsEvent>(true, ic.itemEnum, false, ic.itemEnum, player);
                        eventBus->EmitEvent<UpdateDisplayStatEvent>(player);
                    }
                } else if (playerInventory.heldItemStartingTransformComp.y < 627) { // item from inv
                    // std::cout << "erasing item from inventory at pos " << static_cast<int>(entity.GetComponent<ItemComponent>().lastPosition) << std::endl;
                    playerInventory.inventory.erase(entity.GetComponent<ItemComponent>().lastPosition);
                } 
                entity.GetComponent<ItemComponent>().IdOfHostBag = playerInventory.IdOfOpenBag;
                entity.GetComponent<ItemComponent>().lastPosition = pos;
                // entity.GetComponent<TransformComponent>().position = lootbagPositions.at(pos-1);
                return;
            }
            // std::map<unsigned char, Entity> * originbag;
            if(playerInventory.heldItemStartingTransformComp.y > 627){ // item original bag is a loot bag
                originbag = &registry->GetComponent<LootBagComponent>(playerInventory.IdOfOpenBag).contents;
            } else if(playerInventory.heldItemStartingTransformComp.y < 506){ // item original bag is equipment 
                originbag = &playerInventory.equipment;
            } else { // item original bag is inventory
                originbag = &playerInventory.inventory;
            }

            // "binary search" of possible item-drop locations
            if(my > 627){ // loot bag
                //todo: error-check if lootbag is open
                auto& lootBagContents = registry->GetComponent<LootBagComponent>(playerInventory.IdOfOpenBag).contents;
                if(!playerInventory.viewingBag){
                    transform.position = playerInventory.heldItemStartingTransformComp;
                    assetStore->PlaySound(ERROR);
                    if(transform.position.y > 506){ // item from inventory

                    } else { // item from equipment slot
                        hideIcon(registry, equipmentIcons[entity.GetComponent<ItemComponent>().lastPosition-1]);
                    }
                    return;
                }
                if(my > 686){ // loot bag bottom row
                    if(mx > 877){ // spot 7 or 8
                        if(mx > 934){ // spot 8
                            shiftblock=false;
                            swapOrMoveItemPositions(ic.lastPosition, *originbag, playerInventory.heldItemStartingTransformComp, 8, lootBagContents, {942.5,696.5}, eventBus, assetStore, player, registry, inventoryIcons, equipmentIcons,shift);
                            return;
                        } else { // spot 7
                            shiftblock=false;
                            swapOrMoveItemPositions(ic.lastPosition, *originbag, playerInventory.heldItemStartingTransformComp, 7, lootBagContents, {886.5,696.5}, eventBus, assetStore, player, registry, inventoryIcons, equipmentIcons,shift);
                            return;
                        }
                    } else { // spot 5 or 6
                        if(mx > 819){ // 6
                            shiftblock=false;
                            swapOrMoveItemPositions(ic.lastPosition, *originbag, playerInventory.heldItemStartingTransformComp, 6, lootBagContents, {830.5,696.5}, eventBus, assetStore, player, registry, inventoryIcons, equipmentIcons,shift);
                            return;
                        } else { // 5
                            shiftblock=false;
                            swapOrMoveItemPositions(ic.lastPosition, *originbag, playerInventory.heldItemStartingTransformComp, 5, lootBagContents, {773.5,696.5}, eventBus, assetStore, player, registry, inventoryIcons, equipmentIcons,shift);
                            return;
                        }
                    }
                } else { // loot bag top row
                    if(mx > 877){ // 3 or 4
                        if(mx > 934){ // 4
                            shiftblock=false;
                            swapOrMoveItemPositions(ic.lastPosition, *originbag, playerInventory.heldItemStartingTransformComp, 4, lootBagContents, {942.5,639.5}, eventBus, assetStore, player, registry, inventoryIcons, equipmentIcons,shift);
                            return;
                        } else { // 3
                            shiftblock=false;
                            swapOrMoveItemPositions(ic.lastPosition, *originbag, playerInventory.heldItemStartingTransformComp, 3, lootBagContents, {886.5,639.5}, eventBus, assetStore, player, registry, inventoryIcons, equipmentIcons,shift);
                            return;
                        }
                    } else { //1 or 2 
                        if(mx>819){
                            shiftblock=false;
                            swapOrMoveItemPositions(ic.lastPosition, *originbag, playerInventory.heldItemStartingTransformComp, 2, lootBagContents, {830.5,639.5}, eventBus, assetStore, player, registry, inventoryIcons, equipmentIcons,shift);
                            return;
                        } else {
                            shiftblock=false;
                            swapOrMoveItemPositions(ic.lastPosition, *originbag, playerInventory.heldItemStartingTransformComp, 1, lootBagContents, {773.5,639.5}, eventBus, assetStore, player, registry, inventoryIcons, equipmentIcons,shift);
                            return;
                        }
                    }
                }
            } else { // above loot bag
                if(my > 506){ // player inventory
                    if(my > 561){ // bottom row
                        if(mx > 877){ // 8 or 7
                            if(mx > 934){ // 8
                                shiftblock=false;
                                swapOrMoveItemPositions(ic.lastPosition, *originbag, playerInventory.heldItemStartingTransformComp, 8, playerInventory.inventory, {942.5,572.5}, eventBus, assetStore, player, registry, inventoryIcons, equipmentIcons,shift);
                                return;
                            } else { // 7
                                shiftblock=false;
                                swapOrMoveItemPositions(ic.lastPosition, *originbag, playerInventory.heldItemStartingTransformComp, 7, playerInventory.inventory, {886.5,572.5}, eventBus, assetStore, player, registry, inventoryIcons, equipmentIcons,shift);
                                return;
                            }
                        } else { // 5 or 6
                            if(mx > 819){ // 6
                                shiftblock=false;
                                swapOrMoveItemPositions(ic.lastPosition, *originbag, playerInventory.heldItemStartingTransformComp, 6, playerInventory.inventory, {830.5,572.5}, eventBus, assetStore, player, registry, inventoryIcons, equipmentIcons,shift);
                                return;
                            } else { // 5
                                shiftblock=false;
                                swapOrMoveItemPositions(ic.lastPosition, *originbag, playerInventory.heldItemStartingTransformComp, 5, playerInventory.inventory, {773.5,572.5}, eventBus, assetStore, player, registry, inventoryIcons, equipmentIcons,shift);
                                return;
                            }
                        }
                    } else { // top row
                        if(mx > 877){ // 3 or 4 
                            if(mx > 934){ // 4
                                shiftblock=false;
                                swapOrMoveItemPositions(ic.lastPosition, *originbag, playerInventory.heldItemStartingTransformComp, 4, playerInventory.inventory, {942.5,515.5}, eventBus, assetStore, player, registry, inventoryIcons, equipmentIcons,shift);
                                return;
                            } else { // 3
                                shiftblock=false;
                                swapOrMoveItemPositions(ic.lastPosition, *originbag, playerInventory.heldItemStartingTransformComp, 3, playerInventory.inventory, {886.5,515.5}, eventBus, assetStore, player, registry, inventoryIcons, equipmentIcons,shift);
                                return;
                            }
                        } else {// 1 or 2
                            if(mx > 819){ // 2
                                shiftblock=false;
                                swapOrMoveItemPositions(ic.lastPosition, *originbag, playerInventory.heldItemStartingTransformComp, 2, playerInventory.inventory, {830.5,515.5}, eventBus, assetStore, player, registry, inventoryIcons, equipmentIcons,shift);
                                return;
                            } else { // 1
                                shiftblock=false;
                                swapOrMoveItemPositions(ic.lastPosition, *originbag, playerInventory.heldItemStartingTransformComp, 1, playerInventory.inventory, {773.5,515.5}, eventBus, assetStore, player, registry, inventoryIcons, equipmentIcons,shift);
                                return;
                            }
                        }
                    }
                } else { // equipment slots
                    if(mx > 877){
                        if(mx > 934){ // ring
                            shiftblock=false;
                            swapOrMoveItemPositions(ic.lastPosition, *originbag, playerInventory.heldItemStartingTransformComp, 4, playerInventory.equipment, {942.5,458.5}, eventBus, assetStore, player, registry, inventoryIcons, equipmentIcons,shift);
                            return;
                        } else { // armor
                            shiftblock=false;
                            swapOrMoveItemPositions(ic.lastPosition, *originbag, playerInventory.heldItemStartingTransformComp, 3, playerInventory.equipment, {886.5,458.5}, eventBus, assetStore, player, registry, inventoryIcons, equipmentIcons,shift);
                            return;
                        }
                    } else {
                        if(mx > 819){ // ability
                            shiftblock=false;
                            swapOrMoveItemPositions(ic.lastPosition, *originbag, playerInventory.heldItemStartingTransformComp, 2, playerInventory.equipment, {830.5,458.5}, eventBus, assetStore, player, registry, inventoryIcons, equipmentIcons,0);
                            return;
                        } else { // weapon
                            shiftblock=false;
                            swapOrMoveItemPositions(ic.lastPosition, *originbag, playerInventory.heldItemStartingTransformComp, 1, playerInventory.equipment, {773.5,458.5}, eventBus, assetStore, player, registry, inventoryIcons, equipmentIcons,0);
                            return;
                        }
                    }
                }
            }
        }
    }
    }