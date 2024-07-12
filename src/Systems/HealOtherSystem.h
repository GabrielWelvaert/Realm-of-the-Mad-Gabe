#ifndef HEALOTHERSYSTEM_H
#define HEALOTHERSYSTEM_H

#include "../ECS/ECS.h"
#include "../Utils/factory.h"
#include "../Components/HPMPComponent.h"
#include "../Components/HealOtherComponent.h"
#include "../Components/TextLabelComponent.h"

class HealOtherSystem: public System{
    public:
        HealOtherSystem();

        void Update(std::unique_ptr<Registry>& registry, std::unique_ptr<Factory>& factory);

};

#endif