#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include "../../libs/SDL2/SDL.h"
#include "../Utils/enums.h"
#include <unordered_map>
#include "../Utils/tables.h"

struct SpriteComponent {
    textureEnums assetId; 
    unsigned long width;
    unsigned long height;
    SDL_Rect srcRect;
    unsigned char zIndex;
    bool isFixed; // fixed sprites stay in same spot on screen 
    bool diagonalSprite;
    SDL_RendererFlip flip;

    //assetid, w, h, srcrect{}, zindex, isfixed, diagonal
    inline SpriteComponent(textureEnums const& assetId, unsigned long width, unsigned long height, SDL_Rect const& srcRect, unsigned char const& zindex, bool isFixed, bool diagonalSprite):
    assetId(assetId), width(width), height(height), srcRect(srcRect), zIndex(zindex), isFixed(isFixed), diagonalSprite(diagonalSprite), flip(SDL_FLIP_NONE) {}

    //used for tiles and shit 
    //assetId, width, height, zindex, srcRectx, srcRecty, isfixed
    inline SpriteComponent(textureEnums assetId = LOFIENVIRONMENT, int width = 0, int height = 0, int zindex = 0, int srcRectX = 0, int srcRectY = 0, bool isFixed = false) {
        this->assetId = assetId;
        this->width = width;
        this->height = height;
        this->zIndex = zindex;
        this->srcRect = {srcRectX, srcRectY, width, height};
        this->isFixed = isFixed;
        this->diagonalSprite = false;
        this->flip = SDL_FLIP_NONE;
    }

    // used by player
    inline SpriteComponent(classes cn){
        spritedata data = classToSpriteData.at(cn);
        this->assetId = data.assetId;
        this->width = data.width;
        this->height = data.height;
        this->zIndex = data.zIndex;
        this->srcRect = data.srcRect;
        this->isFixed = data.isFixed;
        this->diagonalSprite = data.diagonalSprite;
        this->flip = SDL_FLIP_NONE;
    }

    // used by monsters 
    inline SpriteComponent(sprites spriteEnum){
        spritedata data = enumToSpriteComponent.at(spriteEnum);
        this->assetId = data.assetId;
        this->width = data.width;
        this->height = data.height;
        this->srcRect = data.srcRect;
        this->zIndex = data.zIndex;
        this->isFixed = data.isFixed;
        this->diagonalSprite = data.diagonalSprite;
        this->flip = SDL_FLIP_NONE;
    }

    inline SpriteComponent(items itemEnum){
        spritedata data = itemEnumTospriteData.at(itemEnum);
        this->assetId = data.assetId;
        this->width = data.width;
        this->height = data.height;
        this->srcRect = data.srcRect;
        this->zIndex = data.zIndex;
        this->isFixed = data.isFixed;
        this->diagonalSprite = data.diagonalSprite;
        this->flip = SDL_FLIP_NONE;
    }

};

#endif