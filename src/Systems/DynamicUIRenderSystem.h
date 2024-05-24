#ifndef DYNAMICUIRENDERSYSTEM_H
#define DYNAMICUIRENDERSYSTEM_H

#include "../../libs/SDL2/SDL.h"
#include "../../libs/SDL2/SDL_image.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/BaseStatComponent.h"
#include "../Components/SpeedStatComponent.h"
#include "../Components/OffenseStatComponent.h"
#include "../Components/DynamicUIEntityComponent.h"
#include "../Components/HPMPComponent.h"
#include "../Utils/tables.h"
#include <algorithm>

/*
This system is responsible for updating the xp,hp, and mp bars of the UI
*/

// this system updates the parts of the UI that are made of rectangles. 
class DynamicUIRenderSystem: public System{
    private:

    public:

        DynamicUIRenderSystem();

        void sortEntities();

        void Update(SDL_Renderer* renderer, Entity player);
};

#endif