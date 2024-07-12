#include "DistanceToPlayerSystem.h"

DistanceToPlayerSystem::DistanceToPlayerSystem(){
    RequireComponent<DistanceToPlayerComponent>();
    RequireComponent<TransformComponent>();
    RequireComponent<BoxColliderComponent>();
    RequireComponent<SpriteComponent>();
}

void DistanceToPlayerSystem::SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
    eventBus->SubscribeToEvent<ScepterUseEvent>(this, &DistanceToPlayerSystem::onScepterUse);
    eventBus->SubscribeToEvent<SkullUseEvent>(this, &DistanceToPlayerSystem::onSkullUse);
}

void DistanceToPlayerSystem::ascendingSort(){
    auto& entities = GetSystemEntities();
    std::sort(entities.begin(), entities.end(),  [](const Entity& e1, const Entity& e2){
        const auto& e1Distance = e1.GetComponent<DistanceToPlayerComponent>().distanceToPlayer;
        const auto& e2Distance = e2.GetComponent<DistanceToPlayerComponent>().distanceToPlayer;
        return e1Distance < e2Distance;
    });
}

void DistanceToPlayerSystem::onScepterUse(ScepterUseEvent& event){
    // setup
    std::unordered_set<int> alreadyInflicted;
    const auto& playerTransform = event.player.GetComponent<TransformComponent>();
    const auto& playerPos = playerTransform.position;
    const auto& playerSprite = event.player.GetComponent<SpriteComponent>();
    const auto& scepterData = event.player.GetComponent<ScepterComponent>();
    glm::vec2 playerCenter = spriteCenter(playerTransform, playerSprite);
    ascendingSort(); // sort monsters in ascending order of distance from player
    const auto& entities = GetSystemEntities(); // all non-invisible monsters will be here!
    glm::vec2 target = {event.mx + event.camera.x, event.my + event.camera.y}; // global mouse position
    constexpr int maxDistanceBetweenTargets = 300; // lightning can travel to next target if it is within this radius
    float playerToMouseDistance = glm::distance(target, playerPos);
    float playerToMouseAngle = angleDegreesTwoPoints(playerCenter, target);
    constexpr float peripheralMax = 50.0f; 
    constexpr int maxDistanceForGenesis = 750;

    // chain lightning algorithm
    // first get genesis monster by finding first monster within angle of mouse arcgap 
    float distanceToCurrentNodeFromPlayer; // distance to current node from the player
    glm::vec2 currentNodeCenter; // holds center positon of node currently inflicted by chain lightning
    for(auto& entity: entities){ // searching in ascending distance from player, find first monster within 20 degrees of mouse position
        const auto& monsterTransform = entity.GetComponent<TransformComponent>();
        const auto& monsterSprite = entity.GetComponent<SpriteComponent>();
        distanceToCurrentNodeFromPlayer = entity.GetComponent<DistanceToPlayerComponent>().distanceToPlayer;
        currentNodeCenter = spriteCenter(monsterTransform, monsterSprite); 
        if(distanceToCurrentNodeFromPlayer <= maxDistanceForGenesis){ // radius where genesis is allowed to exist
            float playerToMonsterAngle = angleDegreesTwoPoints(playerPos, currentNodeCenter);
            // float angleDifference = abs(playerToMouseAngle - playerToMonsterAngle);
            float angleDifference = std::abs(std::fmod(playerToMouseAngle - playerToMonsterAngle + 540.0f, 360.0f) - 180.0f);
            if(distanceToCurrentNodeFromPlayer < 75.0f || angleDifference <= peripheralMax){ // genesis node obtained
                Entity particle = event.factory->spawnScepterParticles(event.player, event.registry, playerCenter, currentNodeCenter, scepterData.damage); // last particle has projectile component
                event.eventBus->EmitEvent<ProjectileDamageEvent>(particle, entity, event.eventBus, event.registry, event.assetStore, event.factory);
                alreadyInflicted.insert(entity.GetId());
                break;
            } 
        } else { // eligibility radius exited; no more candidates possible
            break;
        }
    }
    if(alreadyInflicted.empty()){ // no eligible genesis was obtained, exit function
        event.factory->spawnScepterFailParticles(event.registry, playerCenter, target);
        return;
    }

    // chain lightning to successive monsters
    int chain = 1;
    for(int i = 0; i < scepterData.maxNumberTargets - 1; i++){
        float bestCandidateDistanceToLastNode = std::numeric_limits<float>::max();
        float bestCandidateDistanceToPlayer;
        int bestCandidateId = -1;
        Entity victim;
        glm::vec2 bestCandidateCenter;
        for(auto& entity: entities){ // search outward 
            int monsterId = entity.GetId();
            const auto& candidateDistanceToPlayer = entity.GetComponent<DistanceToPlayerComponent>().distanceToPlayer;
            if(candidateDistanceToPlayer < distanceToCurrentNodeFromPlayer - maxDistanceBetweenTargets || alreadyInflicted.find(monsterId) != alreadyInflicted.end()){ // ineligible candidate
                continue;
            }
            const auto& monsterTransform = entity.GetComponent<TransformComponent>();
            const auto& monsterSprite = entity.GetComponent<SpriteComponent>();
            glm::vec2 candidateCenter = spriteCenter(monsterTransform, monsterSprite);
            float distanceToCandidateFromCurrentNode = glm::distance(currentNodeCenter, candidateCenter);
            if(distanceToCandidateFromCurrentNode < maxDistanceBetweenTargets){ // candidate is withing chain lightning radius
                if(distanceToCandidateFromCurrentNode < bestCandidateDistanceToLastNode){ // is this candidate the closests candidate found thus far?
                    bestCandidateDistanceToLastNode = distanceToCandidateFromCurrentNode;
                    bestCandidateDistanceToPlayer = candidateDistanceToPlayer; 
                    bestCandidateId = monsterId;
                    bestCandidateCenter = candidateCenter;
                    victim = entity;
                }
            }
            if(candidateDistanceToPlayer > distanceToCurrentNodeFromPlayer + maxDistanceBetweenTargets){ // exited eligible radius for next node
                break; // exit for loop because no more eligible candidates must exist
            }
        }

        if(bestCandidateId != -1){ // a new node was obtained. update values representing current node
            Entity particle = event.factory->spawnScepterParticles(event.player, event.registry, currentNodeCenter, bestCandidateCenter, scepterData.damage - scepterData.damageReductionPerTarget * chain);
            chain++;
            event.eventBus->EmitEvent<ProjectileDamageEvent>(particle, victim, event.eventBus, event.registry, event.assetStore, event.factory);
            currentNodeCenter = bestCandidateCenter;
            alreadyInflicted.insert(bestCandidateId);
            distanceToCurrentNodeFromPlayer = bestCandidateDistanceToPlayer;
        } else { // no further nodes obtained, exit function
            return;
        }
    }
}

void DistanceToPlayerSystem::onSkullUse(SkullUseEvent& event){
    int skullTracker = 0;
    const auto& playerTransform = event.player.GetComponent<TransformComponent>();
    const auto& playerPos = playerTransform.position;
    const auto& playerSprite = event.player.GetComponent<SpriteComponent>();
    const auto& skullData = event.player.GetComponent<SkullComponent>();
    glm::vec2 playerCenter = spriteCenter(playerTransform, playerSprite);
    float radius = skullData.radius * 64.0f;
    ascendingSort(); // sort monsters in ascending order of distance from player
    const auto& entities = GetSystemEntities(); // all non-invisible monsters will be here!
    glm::vec2 target = {event.mx + event.camera.x, event.my + event.camera.y}; // global mouse position
    float distanceToMouse = glm::distance(target, playerPos);
    // find all entities within radius (skull range) of target (mouse)
    for(auto& entity: entities){
        float distanceToPlayer = entity.GetComponent<DistanceToPlayerComponent>().distanceToPlayer;
        if(distanceToPlayer < distanceToMouse - radius){ // too close
            continue; 
        } else if(distanceToPlayer > distanceToMouse + radius ){ // too far; no more candidates
            break;
        } 
        const auto& monsterTransform = entity.GetComponent<TransformComponent>();
        const auto& monsterSprite = entity.GetComponent<SpriteComponent>();
        glm::vec2 monsterCenter = spriteCenter(monsterTransform, monsterSprite); 
        if(glm::distance(monsterCenter, target) <= radius){
            Entity simulatedProjectile = event.registry->CreateEntity();
            switch(skullData.itemEnum){
                case ELECTRICSKULL:{
                    simulatedProjectile.AddComponent<ProjectileComponent>(skullData.damage,INT_MAX,false,event.player, 4,NONESPRITE, true, SLOWED, 4000, false);
                    event.eventBus->EmitEvent<ProjectileDamageEvent>(simulatedProjectile,entity,event.eventBus,event.registry,event.assetStore,event.factory,&skullTracker);
                } break;
                default:{
                    simulatedProjectile.AddComponent<ProjectileComponent>(skullData.damage,INT_MAX,false,event.player, 4,NONESPRITE);
                    event.eventBus->EmitEvent<ProjectileDamageEvent>(simulatedProjectile,entity,event.eventBus,event.registry,event.assetStore,event.factory,&skullTracker);
                } break;
            }
        }

    }
    auto& HPMP = event.player.GetComponent<HPMPComponent>();
    HPMP.activehp += skullTracker;
    int amount;
    if(HPMP.activehp > HPMP.maxhp){
        amount = skullTracker - (static_cast<int>(HPMP.activehp) - static_cast<int>(HPMP.maxhp));
        HPMP.activehp = HPMP.maxhp;        
    } else {
        amount = skullTracker;
    }
    if(amount > 0){
        Entity text = event.registry->CreateEntity();
        text.AddComponent<TextLabelComponent>("+" + std::to_string(amount),"damagefont",xpgreen,false,350,event.player.GetId(),event.player.GetCreationId());
        text.AddComponent<TransformComponent>(playerPos);  
    }
    event.factory->spawnNecromancerParticles(event.registry, target, radius);
}

// distanceToPlayer stores distance from center of sprite!
void DistanceToPlayerSystem::Update(const glm::vec2& playerCenter){
    for(auto& entity: GetSystemEntities()){
        auto& monsterTransform = entity.GetComponent<TransformComponent>();
        const auto& monsterSprite = entity.GetComponent<SpriteComponent>();
        monsterTransform.center = spriteCenter(monsterTransform, monsterSprite);
        entity.GetComponent<DistanceToPlayerComponent>().distanceToPlayer = glm::distance(playerCenter, monsterTransform.center);
    }
}