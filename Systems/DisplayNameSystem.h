#ifndef DISPLAYNAMESYSTEM_H
#define DISPLAYNAMESYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/TextLabelComponent.h"
#include "../Components/DisplayNameComponent.h"
#include "../Events/UpdateDisplayNameEvent.h"
#include "../Utils/Xoshiro256.h"

class DisplayNameSystem: public System{
    private:
        Xoshiro256 RNG;

    public:
        DisplayNameSystem();
        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus);
        void onUpdateName(UpdateDisplayNameEvent& event);

};
#endif