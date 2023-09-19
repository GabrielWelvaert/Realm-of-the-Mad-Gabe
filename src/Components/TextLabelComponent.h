#ifndef TEXTLABELCOMPONENT_H
#define TEXTLABELCOMPONENT_H

#include "../../libs/glm/glm.hpp"
#include "../../libs/SDL2/SDL_ttf.h"
#include "../../libs/SDL2/SDL.h"
#include <string>

struct TextLabelComponent{
    std::string text;
    std::string assetId;
    SDL_Color color; 
    bool isFixed; //follow camera or not
    unsigned long msDuration;
    Uint32 birthTime;
    int parentId; //if tied to an entity, store its id here
    int parentCreationId; 
    bool spawnframe;
    int textwidth;
    int textheight;
    bool invisible;

    TextLabelComponent(std::string text = "", 
        std::string assetId = "damagefont", 
        const SDL_Color& color = {185,185,185}, 
        bool isFixed = true, 
        unsigned long msDuration = 500, 
        int parentId = 0, 
        int parentCreationId = 0){
        
        this->text = text;
        this->assetId = assetId;
        this->color = color;
        this->isFixed = isFixed;
        this->msDuration = msDuration;
        this->birthTime = SDL_GetTicks();
        this->parentId = parentId;
        this->parentCreationId = parentCreationId;
        this->spawnframe = true;
        this->invisible = false;
    }

};

#endif