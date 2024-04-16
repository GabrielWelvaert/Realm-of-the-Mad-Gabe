#include "AbilitySystem.h"


AbilitySystem::AbilitySystem(){
    RequireComponent<AbilityComponent>();
}

void AbilitySystem::SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
    eventBus->SubscribeToEvent<EquipAbilityEvent>(this, &AbilitySystem::onAbilityEquip);
    eventBus->SubscribeToEvent<TomeUseEvent>(this, &AbilitySystem::onTomeUse);
    eventBus->SubscribeToEvent<QuiverUseEvent>(this, &AbilitySystem::onQuiverUse);
    eventBus->SubscribeToEvent<HelmUseEvent>(this, &AbilitySystem::onHelmUse);
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
    projectile.Group(PROJECTILE);
    
}

void AbilitySystem::onHelmUse(HelmUseEvent& event){
    const auto& duration = event.player.GetComponent<HelmComponent>().berserkDuration;
    event.eventbus->EmitEvent<StatusEffectEvent>(event.player, BERSERK, event.eventbus, event.registry, duration);
    event.eventbus->EmitEvent<StatusEffectEvent>(event.player, SPEEDY, event.eventbus, event.registry, duration);
}

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
            break;
        }
        case KNIGHT: {
            auto& shield = player.GetComponent<ShieldComponent>();
            auto data = itemEnumToShieldData.at(event.itemEnum);
            shield.debuff = data.debuff;
            shield.debuffDuration = data.debuffDuration;
            break;
        }
        case ROGUE: {
            auto& cloak = player.GetComponent<CloakComponent>();
            auto data = itemEnumToHelmData.at(event.itemEnum);
            cloak.invisibilityDuration = data.duration;
            break;
        }
    }

}