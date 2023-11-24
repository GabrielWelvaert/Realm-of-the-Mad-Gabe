#ifndef RENDERTEXTSYSTEM_H
#define RENDERTEXTSYSTEM_H

#include "../ECS/ECS.h"
#include "../../libs/glm/glm.hpp"
#include "../Components/TextLabelComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../AssetStore/AssetStore.h"
#include "../Utils/colors.h"

/*
this system is responsible for rendering text
*/

class RenderTextSystem: public System{
    private:
        std::unordered_map<std::string, SDL_Texture*> textTextures;

    public:
        RenderTextSystem();
        void killTextures();

        void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, const SDL_Rect& camera, std::unique_ptr<Registry>& registry);
};

#endif 