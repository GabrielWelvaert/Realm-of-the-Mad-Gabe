#include "AbilitySystem.h"


AbilitySystem::AbilitySystem(){
    RequireComponent<AbilityComponent>();
    for(int i = 0; i < 20; i++){ // destination positions for spell velocity calculations
        spellAngles[i] = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(20);
    }
}

void AbilitySystem::SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
    eventBus->SubscribeToEvent<EquipAbilityEvent>(this, &AbilitySystem::onAbilityEquip);
    eventBus->SubscribeToEvent<TomeUseEvent>(this, &AbilitySystem::onTomeUse);
    eventBus->SubscribeToEvent<QuiverUseEvent>(this, &AbilitySystem::onQuiverUse);
    eventBus->SubscribeToEvent<HelmUseEvent>(this, &AbilitySystem::onHelmUse);
    eventBus->SubscribeToEvent<SpellUseEvent>(this, &AbilitySystem::onSpellUse);
    eventBus->SubscribeToEvent<CloakUseEvent>(this, &AbilitySystem::onCloakUse);
    eventBus->SubscribeToEvent<ShieldUseEvent>(this, &AbilitySystem::onShieldUse);
}

void AbilitySystem::onTomeUse(TomeUseEvent& event){
    auto& HPMP = event.player.GetComponent<HPMPComponent>();
    const auto& tome = event.player.GetComponent<TomeComponent>();
    HPMP.activehp += tome.hp;
    if(HPMP.activehp > HPMP.maxhp){
        displayHealText(event.registry, event.player.GetComponent<TransformComponent>().position, tome.hp - (static_cast<int>(HPMP.activehp) - static_cast<int>(HPMP.maxhp)), event.player);
        HPMP.activehp = HPMP.maxhp;
    } else {
        displayHealText(event.registry, event.player.GetComponent<TransformComponent>().position, tome.hp, event.player);
    }
    switch(tome.tomeEnum){
        case CHICKENTOME:{
            event.eventbus->EmitEvent<StatusEffectEvent>(event.player, SPEEDY, event.eventbus, event.registry, 5000);
            event.eventbus->EmitEvent<StatusEffectEvent>(event.player, INVULNERABLE, event.eventbus, event.registry, 1200);
        } break;
        case ARCTOME:{
            const auto& player = event.player;
            const auto& playerpos = player.GetComponent<TransformComponent>().position;
            glm::vec2 originVelocity;
            float rotationDegrees = getRotationFromCoordiante(
                1024, 
                playerpos.x, 
                playerpos.y, 
                event.mx + event.camera.x-24, 
                event.my + event.camera.y-36,
                originVelocity,
                false);
            Entity projectile = event.registry->CreateEntity();
            projectile.AddComponent<RidigBodyComponent>(originVelocity);
            projectile.AddComponent<SpriteComponent>(LOFIOBJ3, 8, 8, SDL_Rect({8*1, 8*22, 8,8}), 3, false, false);
            projectile.AddComponent<BoxColliderComponent>(32,32,glm::vec2(8,8));
            projectile.AddComponent<TransformComponent>(glm::vec2(playerpos.x, playerpos.y+10), glm::vec2(6.0,6.0), rotationDegrees);
            projectile.AddComponent<ProjectileComponent>(1000, 1000, true, player, 4, NONESPRITE, false, QUIET, 3000, false);
            projectile.AddComponent<LinearProjectileComponent>();
            projectile.Group(PROJECTILE);
        } break;
    }
}

void AbilitySystem::onQuiverUse(QuiverUseEvent& event){
    const auto& player = event.player;
    const auto& quiver = player.GetComponent<QuiverComponent>();
    const auto& playerpos = player.GetComponent<TransformComponent>().position;
    glm::vec2 originVelocity;
    float rotationDegrees = getRotationFromCoordiante(
        1024, 
        playerpos.x, 
        playerpos.y, 
        event.mx + event.camera.x-24, 
        event.my + event.camera.y-36,
        originVelocity,
        true);
    int damage = RNG.randomFromRange(quiver.minDamage, quiver.maxDamage);
    Entity projectile = event.registry->CreateEntity();
    projectile.AddComponent<RidigBodyComponent>(originVelocity);
    projectile.AddComponent<SpriteComponent>(quiver.texture, 8, 8, quiver.srcRect, 3, false, true);
    projectile.AddComponent<BoxColliderComponent>(8,8,glm::vec2(16,16));
    projectile.AddComponent<TransformComponent>(glm::vec2(playerpos.x, playerpos.y+10), glm::vec2(6.0,6.0), rotationDegrees);
    projectile.AddComponent<ProjectileComponent>(damage, 1000, true, player, 4, NONESPRITE, true, quiver.debuff, 3000, false);
    projectile.AddComponent<LinearProjectileComponent>();
    projectile.Group(PROJECTILE);
    if(quiver.quiverEnum == DOOMQUIVER){
        event.eventbus->EmitEvent<StatusEffectEvent>(event.player, SLOWED, event.eventbus, event.registry, 1000);
    }
    
}


// similar to PEC::update. spawns projectiles which inflict a status debuff
void AbilitySystem::onShieldUse(ShieldUseEvent& event){
    const auto& player = event.player;
    const auto& shield = player.GetComponent<ShieldComponent>();
    const auto& playerpos = player.GetComponent<TransformComponent>().position;
    glm::vec2 originVelocity;
    float rotationDegrees = getRotationFromCoordiante(
        1024, 
        playerpos.x, 
        playerpos.y, 
        event.mx + event.camera.x - 24,
        event.my + event.camera.y - 36,
        originVelocity, true
    );
    int damage = RNG.randomFromRange(shield.minDamage, shield.maxDamage);
    int shots = shield.numshots;
    const SDL_Rect srcRect = {8*3, 8*1, 8, 8};
    float arcgap = 12 * shots;
    float realgap;
    const int duration = 206; //206

    Entity projectile;

    // hey its pec update!
    if(shots % 2 == 1){
        projectile = event.registry->CreateEntity();
        projectile.AddComponent<RidigBodyComponent>(originVelocity);
        projectile.AddComponent<SpriteComponent>(LOFIOBJ2, 8, 8, srcRect, 3, false, true);
        projectile.AddComponent<BoxColliderComponent>(32,32,glm::vec2(8,8));
        projectile.AddComponent<TransformComponent>(glm::vec2(playerpos.x, playerpos.y+10), glm::vec2(6.0,6.0), rotationDegrees);
        projectile.AddComponent<ProjectileComponent>(damage, duration, true, player, 4, NONESPRITE, true, shield.debuff, shield.debuffDuration, false);
        projectile.AddComponent<LinearProjectileComponent>();
        projectile.Group(PROJECTILE);
        if(shots == 1){return;}
        realgap = arcgap / shots;
        shots -= 1;
        for(int i = 1; i <= shots / 2; i++){
            projectile = event.registry->CreateEntity();
            projectile.AddComponent<RidigBodyComponent>();
            auto& velocity1 = projectile.GetComponent<RidigBodyComponent>().velocity;
            projectileVelocityArcGap(originVelocity, rotationDegrees, realgap*i, velocity1);
            projectile.AddComponent<SpriteComponent>(LOFIOBJ2, 8, 8, srcRect, 3, false, true);
            projectile.AddComponent<BoxColliderComponent>(32,32,glm::vec2(8,8));
            projectile.AddComponent<TransformComponent>(glm::vec2(playerpos.x, playerpos.y+10), glm::vec2(6.0,6.0), rotationDegrees + realgap*i);
            projectile.AddComponent<ProjectileComponent>(damage, duration, true, player, 4, NONESPRITE, true, shield.debuff, shield.debuffDuration, false);
            projectile.AddComponent<LinearProjectileComponent>();
            projectile.Group(PROJECTILE);

            projectile = event.registry->CreateEntity();
            projectile.AddComponent<RidigBodyComponent>();
            auto& velocity2 = projectile.GetComponent<RidigBodyComponent>().velocity;
            projectileVelocityArcGap(originVelocity, rotationDegrees, realgap*-i, velocity2);
            projectile.AddComponent<SpriteComponent>(LOFIOBJ2, 8, 8, srcRect, 3, false, true);
            projectile.AddComponent<BoxColliderComponent>(32,32,glm::vec2(8,8));
            projectile.AddComponent<TransformComponent>(glm::vec2(playerpos.x, playerpos.y+10), glm::vec2(6.0,6.0), rotationDegrees + realgap*-i);
            projectile.AddComponent<ProjectileComponent>(damage, duration, true, player, 4, NONESPRITE, true, shield.debuff, shield.debuffDuration, false);
            projectile.AddComponent<LinearProjectileComponent>();
            projectile.Group(PROJECTILE);
        }
    } else {
        realgap = arcgap / (shots-1);
        for(int i = 0; i < shots/2; i++){
            bool first = i != 0;

            projectile = event.registry->CreateEntity();
            projectile.AddComponent<RidigBodyComponent>();
            auto& velocity1 = projectile.GetComponent<RidigBodyComponent>().velocity;
            projectileVelocityArcGap(originVelocity, rotationDegrees, (realgap/2) + realgap*i*first, velocity1);
            projectile.AddComponent<SpriteComponent>(LOFIOBJ2, 8, 8, srcRect, 3, false, true);
            projectile.AddComponent<BoxColliderComponent>(32,32,glm::vec2(8,8));
            projectile.AddComponent<TransformComponent>(glm::vec2(playerpos.x, playerpos.y+10), glm::vec2(6.0,6.0), (rotationDegrees + realgap/2) + realgap*i*first);
            projectile.AddComponent<ProjectileComponent>(damage, duration, true, player, 4, NONESPRITE, true, shield.debuff, shield.debuffDuration, false);
            projectile.AddComponent<LinearProjectileComponent>();
            projectile.Group(PROJECTILE);

            projectile = event.registry->CreateEntity();
            projectile.AddComponent<RidigBodyComponent>();
            auto& velocity2 = projectile.GetComponent<RidigBodyComponent>().velocity;
            projectileVelocityArcGap(originVelocity, rotationDegrees, (realgap/-2) + realgap*-i*first, velocity2);
            projectile.AddComponent<SpriteComponent>(LOFIOBJ2, 8, 8, srcRect, 3, false, true);
            projectile.AddComponent<BoxColliderComponent>(32,32,glm::vec2(8,8));
            projectile.AddComponent<TransformComponent>(glm::vec2(playerpos.x, playerpos.y+10), glm::vec2(6.0,6.0), (rotationDegrees - realgap/2) + realgap*-i*first);
            projectile.AddComponent<ProjectileComponent>(damage, duration, true, player, 4, NONESPRITE, true, shield.debuff, shield.debuffDuration, false);
            projectile.AddComponent<LinearProjectileComponent>();
            projectile.Group(PROJECTILE);
        }
    }
}

void AbilitySystem::onSpellUse(SpellUseEvent& event){
    const auto& player = event.player;
    const auto& spell = player.GetComponent<SpellComponent>();
    const int spawnOffset = 18;
    glm::vec2 startPos = {event.mx + event.camera.x - spawnOffset, event.my + event.camera.y - spawnOffset}; // offset so it spawns at mouse tip 
    glm::vec2 originVelocity;
    int damage = RNG.randomFromRange(spell.minDamage, spell.maxDamage);
    int spellspeed = 1024;
    int spellDuration = 1000;
    int j = 0;
    for(int i = 0; i < 20; i++){
        glm::vec2 destPos = {startPos.x + 5 * std::cos(spellAngles[i]), startPos.y + 5 * std::sin(spellAngles[i])};
        Entity projectile = event.registry->CreateEntity();
        // std::cout << "spawning spell projectile with id " << projectile.GetId() << '\n';
        j++;
        switch(spell.spellEnum){
            case CURLYWHIRLYSPELL:{
                spellspeed = 30;
                spellDuration = 5000;
                projectile.AddComponent<OscillatingProjectileComponent>(15, .004, glm::vec2(startPos.x, startPos.y), (i%2 == 0));
                projectile.AddComponent<ProjectileComponent>(damage, spellDuration, false, player, 4, NONESPRITE, true, STUNNED, 3000, false);
                projectile.AddComponent<BoxColliderComponent>(32,32,glm::vec2(4,4));
                projectile.AddComponent<RotationComponent>();
            } break;
            default:{
                projectile.AddComponent<LinearProjectileComponent>(); 
                projectile.AddComponent<ProjectileComponent>(damage, spellDuration, false, player, 4, NONESPRITE, false, QUIET, 0, false);
                projectile.AddComponent<BoxColliderComponent>(8,8,glm::vec2(16,16));
            } break;
        }
        float rotationDegrees = getRotationFromCoordiante(spellspeed, startPos.x, startPos.y, destPos.x, destPos.y, originVelocity, true); 
        projectile.AddComponent<RidigBodyComponent>(originVelocity);
        projectile.AddComponent<SpriteComponent>(spell.texture, 8, 8, spell.srcRect, 3, false, true);
        projectile.AddComponent<TransformComponent>(glm::vec2(startPos.x, startPos.y), glm::vec2(5.0,5.0), rotationDegrees);
        projectile.Group(PROJECTILE);
    }   

}

void AbilitySystem::onHelmUse(HelmUseEvent& event){
    const auto& duration = event.player.GetComponent<HelmComponent>().berserkDuration;
    event.eventbus->EmitEvent<StatusEffectEvent>(event.player, BERSERK, event.eventbus, event.registry, duration);
    event.eventbus->EmitEvent<StatusEffectEvent>(event.player, SPEEDY, event.eventbus, event.registry, duration);
}

void AbilitySystem::onCloakUse(CloakUseEvent& event){
    const auto& cloak = event.player.GetComponent<CloakComponent>();
    event.eventbus->EmitEvent<StatusEffectEvent>(event.player, INVISIBLE, event.eventbus, event.registry, cloak.invisibilityDuration);
    switch(cloak.itemEnum){
        case RETRIBUTIONCLOAK:{
            event.eventbus->EmitEvent<StatusEffectEvent>(event.player, BERSERK, event.eventbus, event.registry, 6000);
            event.eventbus->EmitEvent<StatusEffectEvent>(event.player, SLOWED, event.eventbus, event.registry, 6000);
        } break;
    }
}

// updates ability data. doesnt do anything for stats
void AbilitySystem::onAbilityEquip(EquipAbilityEvent& event){ 
    auto& player = event.player;
    auto& classname = player.GetComponent<ClassNameComponent>().classname;
    auto& ac = player.GetComponent<AbilityComponent>();
    auto newAbilityData = itemEnumToAbilityData.at(event.itemEnum);
    ac.coolDownMS = newAbilityData.cooldown;
    ac.mpRequired = newAbilityData.mprequired;
    ac.abilityEquipped = true;

    switch(classname){
        case ARCHER:{
            auto& quiver = player.GetComponent<QuiverComponent>();
            const auto& newquiverdata = itemEnumToQuiverData.at(event.itemEnum);
            quiver.maxDamage = newquiverdata.maxDamage;
            quiver.minDamage = newquiverdata.minDamage;
            quiver.srcRect = newquiverdata.srcRect;
            quiver.texture = newquiverdata.texture;
            quiver.debuff = newquiverdata.debuff;
            quiver.quiverEnum = event.itemEnum;
            break;
        }
        case PRIEST: {
            auto& tome = player.GetComponent<TomeComponent>();
            tome.tomeEnum = event.itemEnum;
            tome.hp = itemEnumToTomeData.at(event.itemEnum).hp;
            break;
        }
        case WARRIOR: {
            auto& helm = player.GetComponent<HelmComponent>();
            Uint32 duration = itemEnumToHelmData.at(event.itemEnum).duration;
            helm.berserkDuration = duration;
            player.GetComponent<AbilityComponent>().coolDownMS = duration;
            break;
        }
        case WIZARD: {
            auto& spell = player.GetComponent<SpellComponent>();
            auto data = itemEnumToSpellData.at(event.itemEnum);
            spell.minDamage = data.minDamage;
            spell.maxDamage = data.maxDamage;
            spell.texture = data.texture;
            spell.srcRect = data.srcRect;
            spell.spellEnum = event.itemEnum;
            break;
        }
        case KNIGHT: {
            auto& shield = player.GetComponent<ShieldComponent>();
            auto data = itemEnumToShieldData.at(event.itemEnum);
            shield.debuff = data.debuff;
            shield.debuffDuration = data.debuffDuration;
            shield.numshots = data.numshots;
            shield.minDamage = data.minDamage;
            shield.maxDamage = data.maxDamage;
            shield.shieldEnum = event.itemEnum;
            break;
        }
        case ROGUE: {
            auto& cloak = player.GetComponent<CloakComponent>();
            auto data = itemEnumToHelmData.at(event.itemEnum);
            cloak.invisibilityDuration = data.duration;
            cloak.itemEnum = event.itemEnum;
            break;
        }
    }

}