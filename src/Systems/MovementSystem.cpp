#include "MovementSystem.h"

MovementSystem::MovementSystem(){
    RequireComponent<TransformComponent>();
    RequireComponent<RidigBodyComponent>();
    RequireComponent<CollisionFlagComponent>();
    RequireComponent<BoxColliderComponent>();
}

void MovementSystem::SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
    eventBus->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::onCollision);
}

void MovementSystem::onCollision(CollisionEvent& event){
    // WALL IS ALWAYS EVENT.A !!! if it isn't things are going to be very bad!
    auto& transform = event.b.GetComponent<TransformComponent>();
    auto& flags = event.b.GetComponent<CollisionFlagComponent>();
    const auto& activespeed = event.b.GetComponent<SpeedStatComponent>().activespeed;
    auto& lastSideHitFlag = flags.lastSideHitFlag;
    auto& lastlastSideHitFlag = flags.lastlastSideHitflag;
    auto& msLastCollisionFlag = flags.msLastCollisionFlag;
    int move = .0167 * activespeed + 2.495;
    bool firstmove = false;
    int msDoubleCollisionBuffer = 10; // may need to be scaled with deltatime / speed
    sidehit currentSide = event.side;

    if(msLastCollisionFlag == 0){ //firstmove case
        msLastCollisionFlag = SDL_GetTicks();
        flags.lastSideHitFlag = currentSide;
        firstmove = true;
        // flags.idOfWallHit = 0;
    }

    if(!(SDL_GetTicks() - msLastCollisionFlag < msDoubleCollisionBuffer && !firstmove && lastSideHitFlag == currentSide)){ //if clause detects double collision as to stop "double" bumping
        msLastCollisionFlag = SDL_GetTicks();
        lastlastSideHitFlag = lastSideHitFlag;
        lastSideHitFlag = currentSide;
        if(event.side == RIGHTSIDE){ //right
            transform.position.x = std::round(transform.position.x += move  ); 
            flags.collisionFlag = RIGHTSIDE; //right
        }
        else if (event.side == LEFTSIDE){ //left 
            transform.position.x = std::round(transform.position.x -= move  );
            flags.collisionFlag = LEFTSIDE; //left
        } 
        else if (event.side == TOPSIDE){ // top
            transform.position.y = std::round(transform.position.y -= move  );
            flags.collisionFlag = TOPSIDE; //up
        } 
        else {      // down
            transform.position.y = std::round(transform.position.y += move  );
            flags.collisionFlag = BOTTOMSIDE; //down
        }

        // storing Id of wall to later be used to check for re-collision with same wall in update)
        flags.idOfOldWallHit = flags.idOfWallHit;
        flags.idOfWallHit = event.a.GetId();
    }

}

void MovementSystem::Update(const double& deltaTime, std::unique_ptr<Registry>& registry) {
    for(auto& entity: GetSystemEntities()){
        if(entity.HasComponent<StatusEffectComponent>()){
            if(entity.GetComponent<StatusEffectComponent>().effects[PARALYZE]){
                continue; // entity is paralyzed; do not move! 
            }
        }

        auto& rigidbody = entity.GetComponent<RidigBodyComponent>();
        auto& flags = entity.GetComponent<CollisionFlagComponent>();
        auto& transform = entity.GetComponent<TransformComponent>();

        if(entity.HasComponent<SpeedStatComponent>()){ // are there anymore moving entities w/ out a speedstat component...?
            const auto& activespeed = entity.GetComponent<SpeedStatComponent>().activespeed;
            float speedinpixelspersecond = 2.85 * activespeed + 150;
            rigidbody.velocity.x *= speedinpixelspersecond;
            rigidbody.velocity.y *= speedinpixelspersecond;
        }

        // if collionFlag, check if next move will result in collision and release if necessary
        if(flags.collisionFlag != NONESIDE && flags.idOfWallHit != -1){// registry->IdBelongsToGroup(flags.idOfWallHit, WALLBOX)){
            auto aTransform = transform;
            const auto& aCollider = entity.GetComponent<BoxColliderComponent>();
            const auto& bTransform = registry->GetComponent<TransformComponent>(flags.idOfWallHit);
            const auto& bCollider = registry->GetComponent<BoxColliderComponent>(flags.idOfWallHit);

            aTransform.position.x += rigidbody.velocity.x * deltaTime;
            aTransform.position.y += rigidbody.velocity.y * deltaTime;
            bool nextMoveWillHitSameWall = CheckAABBCollision(
                aTransform.position.x + aCollider.offset[0],
                aTransform.position.y + aCollider.offset[1],
                aCollider.width,
                aCollider.height,
                bTransform.position.x + bCollider.offset[0],
                bTransform.position.y + bCollider.offset[1],
                bCollider.width,
                bCollider.height);

            if(!nextMoveWillHitSameWall){ // will not hit same wall; release flag from double collision
                // std::cout << "not gonna hit same wall " << std::endl;
                flags.collisionFlag = NONESIDE;
            }
            if(flags.idOfOldWallHit != flags.idOfWallHit && flags.idOfOldWallHit != -1){// registry->IdBelongsToGroup(flags.idOfOldWallHit, WALLBOX)){//flags.idOfOldWallHit != 0){ // check for corner collision, if re-hit, apply respective flag
                const auto& cCollider = registry->GetComponent<BoxColliderComponent>(flags.idOfOldWallHit);
                const auto& cTransform = registry->GetComponent<TransformComponent>(flags.idOfOldWallHit);
                bool nextMoveWIllCauseCornerCollision = CheckAABBCollision(
                aTransform.position.x + aCollider.offset[0],
                aTransform.position.y + aCollider.offset[1],
                aCollider.width,
                aCollider.height,
                cTransform.position.x + cCollider.offset[0],
                cTransform.position.y + cCollider.offset[1],
                cCollider.width,
                cCollider.height);
                if(nextMoveWIllCauseCornerCollision){ // this corner collision stuff stops phasing thru walls at corners! 
                    flags.collisionFlag = getCollisionSide(aTransform.position.x + aCollider.offset[0],aTransform.position.y + aCollider.offset[1],aCollider.width,aCollider.height,cTransform.position.x + cCollider.offset[0],cTransform.position.y + cCollider.offset[1],cCollider.width,cCollider.height); // wall passed as 2nd in getCollisionSide
                    if((flags.lastSideHitFlag == TOPSIDE && flags.lastlastSideHitflag == LEFTSIDE) || (flags.lastSideHitFlag == LEFTSIDE && flags.lastlastSideHitflag == TOPSIDE)){
                        transform.position.y -= 1;
                        transform.position.x -= 1;
                    } else if (((flags.lastSideHitFlag == TOPSIDE && flags.lastlastSideHitflag == RIGHTSIDE) || (flags.lastSideHitFlag == RIGHTSIDE && flags.lastlastSideHitflag == TOPSIDE))) {
                        transform.position.y -= 1;
                        transform.position.x += 1;
                    } else if (((flags.lastSideHitFlag == BOTTOMSIDE && flags.lastlastSideHitflag == LEFTSIDE) || (flags.lastSideHitFlag == LEFTSIDE && flags.lastlastSideHitflag == BOTTOMSIDE))) {
                        transform.position.y += 1;
                        transform.position.x -= 1;
                    } else {
                        transform.position.y += 1; //BR
                        transform.position.x += 1;
                    }
                }
            
            }
        }

        // y not encompass this inside if flags.collisionFlag != NONESIDE ??
        if (flags.collisionFlag == RIGHTSIDE && rigidbody.velocity.x < 0){ // moving left
            rigidbody.velocity.x = 0.0;
        } else if (flags.collisionFlag == LEFTSIDE && rigidbody.velocity.x > 0){ // moving right
            rigidbody.velocity.x = 0.0;
        } else if (flags.collisionFlag == TOPSIDE && rigidbody.velocity.y > 0){ // moving down
            rigidbody.velocity.y = 0.0;
        } else if (flags.collisionFlag == BOTTOMSIDE && rigidbody.velocity.y < 0) { // moving up
            rigidbody.velocity.y = 0.0;
        } 
        
        transform.position.x += rigidbody.velocity.x * deltaTime;
        transform.position.y += rigidbody.velocity.y * deltaTime;

        // transform.collisionFlag = NONESIDE;
    }
}