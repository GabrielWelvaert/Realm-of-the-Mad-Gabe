#ifndef DYNAMICUIENTITYCOMPONENT_H
#define DYNAMICUIENTITYCOMPONENT_H

#include "../../libs/SDL2/SDL.h"

struct DynamicUIEntityComponent{
    SDL_Rect rect;
    unsigned char r;
    unsigned char g;
    unsigned char b;

    DynamicUIEntityComponent() = default;

    DynamicUIEntityComponent(int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b){
        this->rect = {x,y,w,h};
        this->r = r;
        this->g = g;
        this->b = b;
    }

};

#endif