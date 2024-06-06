#ifndef DEATHACTIONSYSTEM_H
#define DEATHACTIONSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/DeathActionComponent.h"
#include "../Components/HPMPComponent.h"
#include "../Utils/tables.h"
#include "../Utils/factory.h"
#include "../Utils/Xoshiro256.h"


class DeathActionSystem: public System{
    private: 
        Xoshiro256 RNG;
        inline glm::vec2 towerDestination(const glm::vec2& origin, float angleDegrees, float distance){
            float angleRadians = glm::radians(angleDegrees);
            float deltaX = distance * std::cos(angleRadians);
            float deltaY = distance * std::sin(angleRadians);
            glm::vec2 destination = origin + glm::vec2(deltaX, deltaY);
            return destination;
        }

    public:
        DeathActionSystem();
        void Update(std::unique_ptr<Factory>& factory, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetstore, std::vector<BossIds>& bosses);

};



#endif