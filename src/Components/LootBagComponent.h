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

    // returns true if item is in this bag
    bool hasItem(int id){
        for(auto x: contents){
            if(x.second.GetId() == id){
                return true;
            }
        }
        return false;
    }

    void printContents(){
        for(auto x: contents){
            std::cout << static_cast<int>(x.first) << ", " << x.second.GetId() << " ";
        }
        std::cout << std::endl;
    }

    void deleteContents(){
        for(auto & pair: contents){
            pair.second.Kill();
        }
    }

};