#include "KeyboardMovementSystem.h"
#define DIAGONALNERF .85f

KeyboardMovementSystem::KeyboardMovementSystem(){
    RequireComponent<KeyboardControlledComponent>(); //only the player has this.
}

void KeyboardMovementSystem::Update(const std::bitset<5>& keysPressed, int mouseX, int mouseY, SDL_Rect camera, bool space, std::unique_ptr<AssetStore>& assetStore, std::unique_ptr<EventBus>& eventbus, std::unique_ptr<Registry>& registry){
    if(GetSystemEntities().size() == 0){return;}
    const auto& player = GetSystemEntities()[0]; //asumes this system has 1 entity: the player
    auto& sprite = player.GetComponent<SpriteComponent>();
    auto& rigidbody = player.GetComponent<RidigBodyComponent>();
    auto& animation = player.GetComponent<AnimationComponent>();
    const auto& classname = player.GetComponent<ClassNameComponent>().classname;
    const auto& pec = player.GetComponent<ProjectileEmitterComponent>();
    const auto& isShooting = player.GetComponent<isShootingComponent>().isShooting;
    auto& ac = player.GetComponent<AbilityComponent>(); 
    const auto& holdingItem = player.GetComponent<PlayerItemsComponent>().holdingItemLastFrame;
    const auto& confused = player.GetComponent<StatusEffectComponent>().effects[CONFUSED];
    int move;
    
    if(confused){
        move = confusedMoves[keysPressed];
    } else {
        move = moves[keysPressed];
    }

    if(space){ // ability use attempt by player
        auto& ac = player.GetComponent<AbilityComponent>(); 
        auto& activemp = player.GetComponent<HPMPComponent>().activemp;
        Uint32 time = SDL_GetTicks();
        if(ac.abilityEquipped && ( ac.timeLastUsed == 0 || time >= ac.timeLastUsed + ac.coolDownMS) ){ // if ability item equipped and not in cooldown
            if(activemp < ac.mpRequired){ // if not enough mana
                if(!ac.blockNoManaSound){
                    assetStore->PlaySound(NOMANA);
                    ac.blockNoManaSound = true;
                }
            } else { // else, we have enough mana: evoke ability
                const auto& classname = player.GetComponent<ClassNameComponent>().classname;
                switch(classname){ 
                    case PRIEST:{
                        auto& HPMP = player.GetComponent<HPMPComponent>();
                        const auto& tomeType = player.GetComponent<TomeComponent>().tomeEnum;
                        switch(tomeType){
                            case ARCTOME:
                            case CHICKENTOME:{ // user may use tome even at full health
                                activemp -= ac.mpRequired;
                                ac.timeLastUsed = time;
                                eventbus->EmitEvent<TomeUseEvent>(player, registry, assetStore, eventbus,mouseX, mouseY, camera);
                            } break;
                            default:{
                                if(HPMP.activehp < HPMP.maxhp){
                                    activemp -= ac.mpRequired;
                                    ac.timeLastUsed = time;
                                    eventbus->EmitEvent<TomeUseEvent>(player, registry, assetStore, eventbus,mouseX, mouseY, camera);
                                }
                            } break;
                        }
                        break;}
                    case WARRIOR:{
                        activemp -= ac.mpRequired;
                        ac.timeLastUsed = time;
                        eventbus->EmitEvent<HelmUseEvent>(player, eventbus, registry);
                        break;}
                    case ARCHER:{
                        activemp -= ac.mpRequired;
                        ac.timeLastUsed = time;
                        eventbus->EmitEvent<QuiverUseEvent>(player, registry,mouseX, mouseY, camera, eventbus);
                        break;}
                    case WIZARD:{
                        activemp -= ac.mpRequired;
                        ac.timeLastUsed = time;
                        eventbus->EmitEvent<SpellUseEvent>(player, registry,mouseX, mouseY, camera);
                        break;}
                    case KNIGHT:{
                        activemp -= ac.mpRequired;
                        ac.timeLastUsed = time;
                        eventbus->EmitEvent<ShieldUseEvent>(player, registry,mouseX, mouseY, camera);
                        break;}
                    case ROGUE:{
                        activemp -= ac.mpRequired;
                        ac.timeLastUsed = time;
                        eventbus->EmitEvent<CloakUseEvent>(player, eventbus, registry);
                        break;}
                }
            }
        }
    } else {
        ac.blockNoManaSound = false; // player wont be warned about insufficient mana again until they release space bar
    }

    // update stuff based off direction of travel
    switch(move){
        case UP:{ // up
            rigidbody.velocity = glm::vec2(0, -1);
            animation.xmin = 1;
            sprite.srcRect.y = sprite.height * 2 + classname * 24;
            sprite.srcRect.x = sprite.height * 2;
            animation.numFrames = 2;
            sprite.flip = SDL_FLIP_NONE;
        }break;
        case DOWN: { // down
            rigidbody.velocity = glm::vec2(0, 1);
            animation.xmin = 1;
            sprite.srcRect.y = sprite.height * 1 + classname * 24;
            sprite.srcRect.x = sprite.height * 1;
            animation.numFrames = 2;
            sprite.flip = SDL_FLIP_NONE;
        }break;
        case LEFT: { // left
            rigidbody.velocity = glm::vec2(-1, 0);
            animation.xmin = 0;
            sprite.srcRect.y = sprite.height * 0 + classname * 24;
            sprite.srcRect.x = sprite.height * 0;
            animation.numFrames = 2;
            sprite.flip = SDL_FLIP_HORIZONTAL;
        }break;
        case RIGHT: { // right
            animation.xmin = 0;
            sprite.srcRect.y = sprite.height * 0 + classname * 24;
            sprite.srcRect.x = sprite.height * 0;
            animation.numFrames = 2;
            rigidbody.velocity = glm::vec2(1, 0);
            sprite.flip = SDL_FLIP_NONE;
        }break;
        case UPRIGHT:{ // up-right
            rigidbody.velocity = glm::vec2(DIAGONALNERF, -DIAGONALNERF);
            animation.xmin = 1;
            sprite.srcRect.y = sprite.height * 2 + classname * 24;
            sprite.srcRect.x = sprite.height * 2;
            animation.numFrames = 2;
            sprite.flip = SDL_FLIP_NONE;
        }break;
        case UPLEFT:{ // up-left 
            rigidbody.velocity = glm::vec2(-DIAGONALNERF, -DIAGONALNERF);
            animation.xmin = 1;
            sprite.srcRect.y = sprite.height * 2 + classname * 24;
            sprite.srcRect.x = sprite.height * 2;
            animation.numFrames = 2;
            sprite.flip = SDL_FLIP_NONE;
        }break;
        case DOWNRIGHT:{ // down-right
            rigidbody.velocity = glm::vec2(DIAGONALNERF, DIAGONALNERF);
            animation.xmin = 1;
            sprite.srcRect.y = sprite.height * 1 + classname * 24;
            sprite.srcRect.x = sprite.height * 1;
            animation.numFrames = 2;
            sprite.flip = SDL_FLIP_NONE;
        }break;
        case DOWNLEFT:{ // down-left
            rigidbody.velocity = glm::vec2(-DIAGONALNERF, DIAGONALNERF);
            animation.xmin = 1;
            sprite.srcRect.y = sprite.height * 1 + classname * 24;
            sprite.srcRect.x = sprite.height * 1;
            animation.numFrames = 2;
            sprite.flip = SDL_FLIP_NONE;
        }break;
        case NONE:{ // none
            animation.numFrames = 1;
            animation.currentFrame = 1;
            animation.xmin = 0;
            rigidbody.velocity = glm::vec2(0);
        }break;
    }
    // update stuff for if shooting
    if(isShooting && pec.shots > 0){ //previously if keysPressed[4]
        const auto& activedexterity = player.GetComponent<OffenseStatComponent>().activedexterity;
        auto& transform = player.GetComponent<TransformComponent>();
        auto& asc = player.GetComponent<AnimatedShootingComponent>();
        int playerCenterX = transform.position.x + ( (sprite.width * transform.scale.x) / 2);
        int playerCenterY = transform.position.y + sprite.height * transform.scale.y;
        int mousePos = clicked(mouseX, mouseY, playerCenterX, playerCenterY, camera);
        if(mousePos == RIGHTSIDE){ // right
            sprite.srcRect.y = sprite.height * 0 + classname * 24;
            asc.animatedShooting = true;
            sprite.flip = SDL_FLIP_NONE;
        } else if (mousePos == BOTTOMSIDE){ // below 
            asc.animatedShooting = false;
            sprite.flip = SDL_FLIP_NONE;
            sprite.srcRect.y = sprite.height * 1 + classname * 24;
        } else if (mousePos == TOPSIDE) { // above
            sprite.srcRect.y = sprite.height * 2 + classname * 24;
            asc.animatedShooting = false;
            sprite.flip = SDL_FLIP_NONE;
        } else { // mousePos == "left"
            sprite.srcRect.y = sprite.height * 0 + classname * 24;
            sprite.flip = SDL_FLIP_HORIZONTAL;
            asc.animatedShooting = true;
        }
        animation.numFrames = 2;
        animation.xmin = 4;
        animation.frameSpeedRate = (.08666 * activedexterity + 1.5) * 2;
    } else {
        const auto& activespeed = player.GetComponent<SpeedStatComponent>().activespeed;
        auto& asc = player.GetComponent<AnimatedShootingComponent>();
        if(activespeed <= 20){
            animation.frameSpeedRate = 2;
        } else {
            animation.frameSpeedRate = activespeed/10;    
        }
        asc.animatedShooting = false;
    }

}
