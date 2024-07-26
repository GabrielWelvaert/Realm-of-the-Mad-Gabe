#include "RenderTextSystem.h"
#include <cstdint> 

RenderTextSystem::RenderTextSystem(){
    RequireComponent<TextLabelComponent>();
    RequireComponent<TransformComponent>();
}

void RenderTextSystem::killTextures(){ // not sure why valgrind reports invalid reads here!
    for(auto& pair: textTextures){
        SDL_DestroyTexture(pair.second);
    }
}

void RenderTextSystem::Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, const SDL_Rect& camera, std::unique_ptr<Registry>& registry){
    for (auto& entity: GetSystemEntities()){
        auto& textlabel = entity.GetComponent<TextLabelComponent>();

        if(textlabel.invisible) {continue;}

        // if not fixed (following an entity): if lifespan elapsed or entity being followed has died; kill text
        if(!textlabel.isFixed && ((SDL_GetTicks() - textlabel.msDuration > textlabel.birthTime) || registry->GetCreationIdFromEntityId(textlabel.parentId) != textlabel.parentCreationId)){
            entity.Kill();
            continue;
        }

        auto& position = entity.GetComponent<TransformComponent>().position;     

        SDL_Texture* texture;
        std::string key = textlabel.text + std::to_string(textlabel.color.a) + std::to_string(textlabel.color.b) + std::to_string(textlabel.color.g) + std::to_string(textlabel.color.r);
        if(textTextures.find(key) == textTextures.end()){ // attempts to re-use texture of existing renderText Texture! 
            SDL_Surface* surface = TTF_RenderText_Blended(assetStore->GetFont(textlabel.assetId), textlabel.text.c_str(), textlabel.color);
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);    
            textTextures.insert({key,texture});
        } else {
            texture = textTextures.at(key);
        }

        if(textlabel.spawnframe){
            TTF_SizeText(assetStore->GetFont(textlabel.assetId), textlabel.text.c_str(), &textlabel.textwidth, &textlabel.textheight);
            if(textlabel.isFixed){textlabel.spawnframe = false;}
        }

        // if not fixed, it must be following a monster/player (parent=victim)
        if(!textlabel.isFixed){
            const auto& victimSprite = registry->GetComponent<SpriteComponent>(textlabel.parentId);
            const auto& transform = registry->GetComponent<TransformComponent>(textlabel.parentId);
            position.x = transform.position.x + (static_cast<int>(victimSprite.width) * transform.scale.x /2 - textlabel.textwidth/2);
            if(position.x + textlabel.textwidth - camera.x >= 750){continue;} // dont render if its under the HUD
            if(textlabel.spawnframe){
                position.y = transform.position.y - textlabel.textheight - 4;
                textlabel.spawnframe = false;
            } else {
                float deltaTime = SDL_GetTicks() - textlabel.birthTime;
                position.y = transform.position.y - textlabel.textheight - 4 - deltaTime/10;
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
