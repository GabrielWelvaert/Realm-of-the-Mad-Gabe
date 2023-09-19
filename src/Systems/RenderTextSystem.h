#ifndef RENDERTEXTSYSTEM_H
#define RENDERTEXTSYSTEM_H

#include "../ECS/ECS.h"
#include "../../libs/glm/glm.hpp"
#include "../Components/TextLabelComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../AssetStore/AssetStore.h"

/*
this system is responsible for rendering text, which is different from rendering sprites 
*/

class RenderTextSystem: public System{
    private:
        std::unordered_map<std::string, SDL_Texture*> textTextures;

    public:
        RenderTextSystem(){
            RequireComponent<TextLabelComponent>();
            RequireComponent<TransformComponent>();
        }

        void killTextures(){
            for(auto x: textTextures){
                SDL_DestroyTexture(x.second);
            }
            textTextures.clear();
        }

        void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, const SDL_Rect& camera, std::unique_ptr<Registry>& registry){
        for (auto entity: GetSystemEntities()){
            auto& textlabel = entity.GetComponent<TextLabelComponent>();

            if(textlabel.invisible) {continue;}

            // if not fixed (following an entity) and time to die; kill & continue
            if(!textlabel.isFixed && (SDL_GetTicks() - textlabel.msDuration > textlabel.birthTime)){
                entity.Kill();
                continue;
            }

            auto& position = entity.GetComponent<TransformComponent>().position;     

            SDL_Texture* texture;
            if(textTextures.find(textlabel.text) == textTextures.end()){
                SDL_Surface* surface = TTF_RenderText_Blended(assetStore->GetFont(textlabel.assetId), textlabel.text.c_str(), textlabel.color);
                texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_FreeSurface(surface);    
                textTextures[textlabel.text] = texture;
            } else {
                texture = textTextures.at(textlabel.text);
            }

            if(textlabel.spawnframe){
                SDL_QueryTexture(texture, NULL, NULL, &textlabel.textwidth, &textlabel.textheight); // each char is 8 pixels wide it seems    
                if(textlabel.isFixed){textlabel.spawnframe = false;}
            }

            // if not fixed, it must be following a monster/player (parent=victim)
            if(!textlabel.isFixed && registry->GetCreationIdFromEntityId(textlabel.parentId) == textlabel.parentCreationId){
                const auto& victimSprite = registry->GetComponent<SpriteComponent>(textlabel.parentId);
                const auto& parentPosition = registry->GetComponent<TransformComponent>(textlabel.parentId).position;
                position.x = parentPosition.x + (static_cast<int>(victimSprite.height) * 6 /2 - textlabel.textwidth/2);
                if(textlabel.spawnframe){
                    position.y = parentPosition.y - textlabel.textheight - 4;
                    textlabel.spawnframe = false;
                } else {
                    float deltaTime = SDL_GetTicks() - textlabel.birthTime;
                    position.y = parentPosition.y - textlabel.textheight - 4 - deltaTime/10;
                }
            }

            SDL_Rect dstRect = {
                static_cast<int>(position.x - camera.x * !textlabel.isFixed), 
                static_cast<int>(position.y - camera.y * !textlabel.isFixed),
                textlabel.textwidth,
                textlabel.textheight};
            
            SDL_RenderCopy(renderer, texture, NULL, &dstRect);

        }
    }
};

#endif 