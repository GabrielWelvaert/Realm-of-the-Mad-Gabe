#ifndef DISTANCETOPLAYERSYSTEM_H
#define DISTANCETOPLAYERSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/DistanceToPlayerComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/SkullUseEvent.h"
#include "../Events/ScepterUseEvent.h"
#include "../Components/AbilityComponent.h"
#include "../Components/SpriteComponent.h"
#include <algorithm>
#include <limits>
#include "../Events/ProjectileDamageEvent.h"

class DistanceToPlayerSystem: public System{
    private:
        inline float distance(const glm::vec2& origin, const glm::vec2& destination) {
            return sqrt(pow(origin.x - destination.x, 2) + pow(origin.y - destination.y, 2));
        }

        // return angle between origin (arg1) and dest (arg2)
        inline float angleDegreesTwoPoints(const glm::vec2& origin, const glm::vec2& dest){
            return std::atan2(dest.y - origin.y, dest.x - origin.x) * (180.0 / M_PI);
        }

        inline float angleRadiansTwoPoints(const glm::vec2& origin, const glm::vec2& dest){
            return std::atan2(dest.y - origin.y, dest.x - origin.x);
        }

        inline glm::vec2 spriteCenter(const TransformComponent& t, const SpriteComponent& s){
            return {(t.position.x + ((s.width * t.scale.x) / 2)), t.position.y + ((s.height * t.scale.y) / 2)};
        }

    public:
        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus);
        DistanceToPlayerSystem();
        void Update(const Entity& player);
        void onSkullUse(SkullUseEvent& event);
        void onScepterUse(ScepterUseEvent& event);
        void ascendingSort();
};

#endif