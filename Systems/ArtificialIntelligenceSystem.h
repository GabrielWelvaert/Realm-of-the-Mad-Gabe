#ifndef ARTIFICIALINTELLIGENCESYSTEM_H
#define ARTIFICIALINTELLIGENCESYSTEM_H

#include "../ECS/ECS.h"
#include "../../libs/glm/glm.hpp"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/AnimatedShootingComponent.h"
#include "../../libs/SDL2/SDL.h"
#include <vector>
#include "../Components/PassiveAIComponent.h"
#include "../Components/ChaseAIComponent.h"
#include "../Components/NeutralAIComponent.h"
#include "../Components/TrapAIComponent.h"
#include "../Components/AnimatedChaseAIComponent.h"
#include "../Components/AnimatedNeutralAIComponent.h"
#include "../Components/AnimationComponent.h"
#include "../AssetStore/AssetStore.h"
#include "../Components/StatusEffectComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/HPMPComponent.h"
#include "../AssetStore/AssetStore.h"

/*
These systems are like the KBMS but for monsters; they update sprite-atlas ranges, velocities, and various flags based off of their reaction environmental (player) conditions
*/

// never shoots or chases.
class PassiveAISystem: public System{
    public:
        
        PassiveAISystem();

        void Update(glm::vec2 playerPos, std::unique_ptr<AssetStore>& assetStore);
};

// chases and shooots
class ChaseAISystem: public System{
    public:

        ChaseAISystem();

        void Update(glm::vec2 playerPos, std::unique_ptr<AssetStore>& assetStore);
};

// shoots, does not chase
class NeutralAISystem: public System{
    public:

        NeutralAISystem();
        void Update(glm::vec2 playerPos, std::unique_ptr<AssetStore>& assetStore);
};

class TrapAISystem: public System{
    public:

        TrapAISystem();

        void Update(glm::vec2 playerPos, std::unique_ptr<AssetStore>& assetStore);
};

// chases and shooots (animated)
class AnimatedChaseAISystem: public System{
    public:

        AnimatedChaseAISystem();

        void Update(glm::vec2 playerPos, std::unique_ptr<AssetStore>& assetStore);
};

// shoots, does not chase (animated)
class AnimatedNeutralAISystem: public System{
    public:

        AnimatedNeutralAISystem();

        void Update(glm::vec2 playerPos, std::unique_ptr<AssetStore>& assetStore);
};
#endif 