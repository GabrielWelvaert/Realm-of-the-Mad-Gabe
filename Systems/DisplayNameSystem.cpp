#include "DisplayNameSystem.h"

DisplayNameSystem::DisplayNameSystem(){
    RequireComponent<DisplayNameComponent>();
    RequireComponent<TransformComponent>();
    RequireComponent<TextLabelComponent>();
}

void DisplayNameSystem::SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
    eventBus->SubscribeToEvent(this, &DisplayNameSystem::onUpdateName);
}

void DisplayNameSystem::onUpdateName(UpdateDisplayNameEvent& event){
    Entity& displayTextEntity = GetSystemEntities()[0];
    auto& resizeFlag = displayTextEntity.GetComponent<TextLabelComponent>().spawnframe;
    auto& newName = displayTextEntity.GetComponent<TextLabelComponent>().text;
    newName = "          ";
    int nameIndex = 0;
    bool processInput = true;
    SDL_Keycode key;
    SDL_Event sdlEvent;

    while(processInput){
        while(SDL_PollEvent(&sdlEvent)) {
            key = sdlEvent.key.keysym.sym;
            if (sdlEvent.type == SDL_KEYDOWN && (key == SDLK_ESCAPE || key == SDLK_TAB || key == SDLK_RETURN || key == SDLK_BACKSPACE || key == SDLK_RETURN2)) { 
                switch(key){
                    case SDLK_RETURN:
                    case SDLK_RETURN2 :{
                        processInput = false;
                    } break;
                    case SDLK_BACKSPACE:{
                        if(nameIndex <= 9 && nameIndex > 0){
                            newName[nameIndex-1] = ' ';
                            resizeFlag = true;
                            if(nameIndex > 0){
                                nameIndex --;    
                            }
                        }
                    } break;
                    case SDLK_TAB:
                    case SDLK_ESCAPE:{
                        continue;
                    } break;
                }
                break;
            } else {
                SDL_StartTextInput();
                switch(sdlEvent.type){
                    case SDL_TEXTINPUT:{
                        if(nameIndex <= 9){
                            newName[nameIndex] = *sdlEvent.text.text;
                            resizeFlag = true;
                            if(nameIndex < 9){
                                nameIndex ++;    
                            } else if(nameIndex == 9 && newName[9] != ' '){ // 10 characters detected
                                processInput = false;
                            }
                        }
                    } break;
                }
                break;
                SDL_StopTextInput();
            }
        }
        event.gameRender(); // evoke function pointer to Game::Render()
    }
    if(newName == "          "){
        newName = defaultNames[RNG.randomFromRange(0, defaultNames.size()-1)];
        resizeFlag = true;
    }
    event.characterManager->SaveName(newName);
    event.assetstore->PlaySound(INVENTORY);
}