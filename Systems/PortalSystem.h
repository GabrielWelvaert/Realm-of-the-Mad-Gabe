#ifndef PORTALSYSTEM_H
#define PORTALSYSTEM_H

#include "../ECS/ECS.h"
#include "../../libs/SDL2/SDL.h"
#include "../Utils/tables.h"
#include "../Utils/enums.h"
#include "../Components/PortalComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/PortalCollisionEvent.h"

class PortalSystem: public System{
    private:
        inline bool CheckAABBCollision(double ax, double ay, double aw, double ah, double bx, double by, double bw, double bh){
            return (
                ax < bx + bw &&
                ax + aw > bx &&
                ay < by + bh &&
                ay + ah > by
            );
        }
    public:
        PortalSystem();
        void Update(Entity& player, std::unique_ptr<EventBus>& eventBus, std::unique_ptr<Registry>& registry);


};  

#endif