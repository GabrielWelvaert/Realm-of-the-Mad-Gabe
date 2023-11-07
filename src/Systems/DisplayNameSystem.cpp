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
    // update name
    // call render update for each letter?
}