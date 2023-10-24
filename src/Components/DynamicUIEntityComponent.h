#ifndef DYNAMICUIENTITYCOMPONENT_H
#define DYNAMICUIENTITYCOMPONENT_H

#include "../../libs/SDL2/SDL.h"
#include "../Utils/enums.h"

struct DynamicUIEntityComponent{
    SDL_Rect rect;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    stats statEnum;

    inline DynamicUIEntityComponent() = default;

    inline DynamicUIEntityComponent(int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b, stats statEnum){
        this->rect = {x,y,w,h};
        this->r = r;
        this->g = g;
        this->b = b;
        this->statEnum = statEnum;
    }

};

#endif