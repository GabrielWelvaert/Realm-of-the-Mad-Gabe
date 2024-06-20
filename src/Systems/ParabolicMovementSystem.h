#ifndef PARABOLICMOVEMENTSYSTEM_H
#define PARABOLICMOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/ParabolicMovementComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Utils/factory.h"
#include "../EventBus/EventBus.h"
#include "../EventBus/Event.h"
#include "../Events/LevelUpEvent.h"
#include "../AssetStore/AssetStore.h"
#include "../Utils/DeadPlayer.h"
#include "../Events/AOEBombEvent.h"

class ParabolicMovementSystem: public System{
    public:
        ParabolicMovementSystem();
        void Update(Entity player, const double& deltaTime, 
                        std::unique_ptr<EventBus>& eventBus,
                        std::unique_ptr<Registry>& registry, 
                        std::unique_ptr<AssetStore>& assetStore, 
                        std::unique_ptr<Factory>& factory, 
                        std::function<void(bool, bool, wallTheme)> Setup, 
                        DeadPlayer& deadPlayer, 
                        std::string& activeCharacterID, 
                        std::unique_ptr<CharacterManager>& characterManager);
};

#endif