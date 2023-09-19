#pragma once 

#include "../ECS/ECS.h"
#include "../../libs/SDL2/SDL.h"
#include <map>  
#include "../Components/ItemComponent.h"

struct LootBagComponent{
    Uint32 spawnTime;
    std::map<unsigned char, Entity> contents;
    bool opened;

    LootBagComponent():
    spawnTime(SDL_GetTicks()), opened(false) {}

    unsigned char addItem(Entity item){
        int i = 1;
        while(i < 9){
            if(contents.find(i) == contents.end()){
                // std::cout << "spawning item in slot " << i << std::endl;
                contents[i] = item;
                return i;
            }
            i++;
        }
        item.Kill(); // bag is full, kill item
        return 0;
    }

    void deleteContents(){
        for(auto & pair: contents){
            pair.second.Kill();
        }
    }

    void swapItemPositions(unsigned int item1, unsigned int item2){
        std::pair<unsigned char, Entity> tempitem = *contents.find(item1);
        contents[item1] = contents[item2];
        contents[item2] = tempitem.second;
    }
};