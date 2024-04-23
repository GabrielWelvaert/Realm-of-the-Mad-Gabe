#include "ProjectileEmitSystem.h"

ProjectileEmitSystem::ProjectileEmitSystem(){
    RequireComponent<ProjectileEmitterComponent>();
    RequireComponent<TransformComponent>();
    RequireComponent<SpriteComponent>();
}

void ProjectileEmitSystem::SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
    eventBus->SubscribeToEvent<WeaponEquipEvent>(this, &ProjectileEmitSystem::onPlayerEquippedWeapon);
}

void ProjectileEmitSystem::onPlayerEquippedWeapon(WeaponEquipEvent& event){
    auto& playerPEC = event.player.GetComponent<ProjectileEmitterComponent>();
    const auto& itemEnum = event.itemEnum; 
    const auto& newPECData = itemEnumToPECdata.at(itemEnum);
    playerPEC.duration = newPECData.duration;
    playerPEC.minDamage = newPECData.minDamage;
    playerPEC.damage = newPECData.maxDamage;
    playerPEC.projectileSpeed = newPECData.projectileSpeed;
    playerPEC.piercing = newPECData.piercing;
    playerPEC.shots = newPECData.shots;
    playerPEC.arcgap = newPECData.arcgap;
    playerPEC.spriteassetId = newPECData.spriteasetId;
    playerPEC.spritewidth = newPECData.spriteWidth;
    playerPEC.spriteheight = newPECData.spriteHeight;
    playerPEC.spritesrcRect = newPECData.spriteSrcRect;
    playerPEC.spritezIndex = newPECData.zIndex;
    playerPEC.spritediagonalSprite = newPECData.spritediagonalsprite;
    playerPEC.boxwidth = newPECData.boxdwith;
    playerPEC.boxheight = newPECData.boxheight;
    playerPEC.boxoffset = newPECData.boxoffset;
    playerPEC.oscillation = newPECData.oscillation;
    playerPEC.amplitude = newPECData.amplitude;
    playerPEC.frequency = newPECData.frequency;
}

void ProjectileEmitSystem::Update(std::unique_ptr<Registry>& registry, SDL_Rect camera, int mx, int my, glm::vec2 playerPos, std::unique_ptr<AssetStore>& assetStore) {
    for (auto& entity: GetSystemEntities()){
        auto& PEC = entity.GetComponent<ProjectileEmitterComponent>();
        if(PEC.isShooting && PEC.shots > 0){
            if (SDL_GetTicks() - PEC.lastEmissionTime > PEC.repeatFrequency){ // shoot time; emit projectile!
                const auto& duration = PEC.duration;
                const auto& piercing = PEC.piercing;
                const auto& arcgap = PEC.arcgap;
                int shots = PEC.shots;
                const bool IsPlayer = entity.BelongsToGroup(PLAYER);
                const auto& parentSprite = entity.GetComponent<SpriteComponent>(); // sprite of projectile's parent
                const auto& transform = entity.GetComponent<TransformComponent>(); // transform of projectile's parent
                const auto& isDiagonal = PEC.spritediagonalSprite;
                auto projSpawnX = transform.position.x + ((parentSprite.width * transform.scale.x) / 2) - PEC.spritewidth * projectilescale / 2; 
                auto projSpawnY = transform.position.y + ((parentSprite.height * transform.scale.y) / 2) - PEC.spriteheight * projectilescale / 2; 
                auto projectilePosition = glm::vec2(projSpawnX, projSpawnY); 
                unsigned long damage;
                unsigned char parentGroupEnumInt; // early on, I didn't fully understand how enums work
                int numShotsEmitted = 0; // number of shots emitted this frame used in Oscillating projectile logic

                // get origin data
                float realgap;
                float rotationDegrees = 0.0;
                Entity projectile;
                glm::vec2 originVelocity;
                if(IsPlayer){                                                      
                    int yDestOffset = 18; // 18 perfect for bolts
                    int xDestOffset = 0;
                    switch(PEC.spritewidth){
                        case 8:{ // probably arrows
                            xDestOffset = 18;
                        } break;
                        case 5:{ // wizard bolts
                            yDestOffset = 0;
                            projectilePosition.x += 8;
                        } break;
                    }
                    rotationDegrees = getRotationFromCoordiante(PEC.projectileSpeed, projectilePosition.x, projectilePosition.y, mx+camera.x-xDestOffset, my+camera.y-yDestOffset, originVelocity, isDiagonal);
                    const auto& activeattack = entity.GetComponent<OffenseStatComponent>().activeattack;
                    damage = RNG.randomFromRange(PEC.minDamage, PEC.damage);
                    float damageCalc = static_cast<float>(damage)*((static_cast<float>(activeattack)+25)/50); // must cast to float so math works
                    damage = damageCalc; // damage rounds down
                    parentGroupEnumInt = 4; // hardcoded. must be changed if groups enum is altered!
                    // damage = RNG.randomFromRange(PEC.minDamage, PEC.damage);
                } else{
                    rotationDegrees = getRotationFromCoordiante(PEC.projectileSpeed, projectilePosition.x, projectilePosition.y, playerPos.x+20, playerPos.y+8, originVelocity, isDiagonal); 
                    parentGroupEnumInt = 0; // hardcoded. must be changed if groups enum is altered!
                    damage = PEC.damage;
                    
                }                                                                                                       // should target centerplayer not playerpos?
                
                /*repeated logic incoming. This was one of the earlier systems and its pretty messy. */

                if(shots % 2 == 1){ // odd number of shots; shoot one at origin!
                    projectile = registry->CreateEntity();
                    projectile.AddComponent<RidigBodyComponent>(originVelocity);
                    projectile.AddComponent<SpriteComponent>(PEC.spriteassetId, PEC.spritewidth, PEC.spriteheight, PEC.spritesrcRect, PEC.spritezIndex, PEC.spriteisFixed, PEC.spritediagonalSprite);
                    projectile.AddComponent<BoxColliderComponent>(PEC.boxwidth, PEC.boxheight, PEC.boxoffset);
                    projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(projectilescale,projectilescale), rotationDegrees); //init pos is same as emitter 
                    projectile.AddComponent<ProjectileComponent>(damage, duration, piercing, entity, parentGroupEnumInt, PEC.spriteOfParent, PEC.inflictsStatusEffect, PEC.statusEffect, PEC.durationMS, PEC.ignoresDefense);
                    if(PEC.oscillation){
                        projectile.AddComponent<OscillatingProjectileComponent>(PEC.amplitude, PEC.frequency, projectilePosition, false); // no phase shift if one shot 
                    } else {
                        projectile.AddComponent<LinearProjectileComponent>();
                    }
                    projectile.Group(PROJECTILE);
                    numShotsEmitted++;
                    if(shots == 1){ // only one shot just shoot and origin and exit
                        if(IsPlayer){
                            const auto& classname = entity.GetComponent<ClassNameComponent>().classname; 
                            assetStore->PlaySound(playerSounds[classname]);
                        }
                        PEC.lastEmissionTime = SDL_GetTicks();
                        continue;
                    }
                    realgap = arcgap / shots;
                    shots -= 1;
                    for(int i = 1; i <= shots / 2; i++){ // shoot the rest of the shots in odd numshot case (3,5,7, etc)
                        // next above origin
                        projectile = registry->CreateEntity();
                        projectile.AddComponent<RidigBodyComponent>();
                        auto& velocity1 = projectile.GetComponent<RidigBodyComponent>().velocity;
                        projectileVelocityArcGap(originVelocity, rotationDegrees, realgap*i, velocity1);
                        projectile.AddComponent<SpriteComponent>(PEC.spriteassetId, PEC.spritewidth, PEC.spriteheight, PEC.spritesrcRect, PEC.spritezIndex, PEC.spriteisFixed, PEC.spritediagonalSprite);
                        projectile.AddComponent<BoxColliderComponent>(PEC.boxwidth, PEC.boxheight, PEC.boxoffset);
                        projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(projectilescale,projectilescale), rotationDegrees + realgap*i); //init pos is same as emitter 
                        projectile.AddComponent<ProjectileComponent>(damage, duration, piercing, entity, parentGroupEnumInt, PEC.spriteOfParent, PEC.inflictsStatusEffect, PEC.statusEffect, PEC.durationMS, PEC.ignoresDefense);
                        projectile.AddComponent<LinearProjectileComponent>(); // OSCILLATING SHOTS MAY ONLY EXIST AS 1 OR 2 SHOTS; IT SHOULD NEVER HAPPEN HERE!
                        projectile.Group(PROJECTILE);
                        numShotsEmitted++;
                        
                        // next below origin
                        projectile = registry->CreateEntity();
                        projectile.AddComponent<RidigBodyComponent>();
                        auto& velocity2 = projectile.GetComponent<RidigBodyComponent>().velocity;
                        projectileVelocityArcGap(originVelocity, rotationDegrees, realgap*-i, velocity2);
                        projectile.AddComponent<SpriteComponent>(PEC.spriteassetId, PEC.spritewidth, PEC.spriteheight, PEC.spritesrcRect, PEC.spritezIndex, PEC.spriteisFixed, PEC.spritediagonalSprite);
                        projectile.AddComponent<BoxColliderComponent>(PEC.boxwidth, PEC.boxheight, PEC.boxoffset);
                        projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(projectilescale,projectilescale), rotationDegrees + realgap*-i); //init pos is same as emitter 
                        projectile.AddComponent<ProjectileComponent>(damage, duration, piercing, entity, parentGroupEnumInt, PEC.spriteOfParent, PEC.inflictsStatusEffect, PEC.statusEffect, PEC.durationMS, PEC.ignoresDefense);
                        projectile.AddComponent<LinearProjectileComponent>(); // OSCILLATING SHOTS MAY ONLY EXIST AS 1 OR 2 SHOTS; IT SHOULD NEVER HAPPEN HERE!
                        projectile.Group(PROJECTILE);
                        numShotsEmitted++;
                    }

                } else { // even number of shots
                    realgap = arcgap / (shots-1);
                    for(int i = 0; i < shots/2; i++){
                        bool first = i != 0;

                        projectile = registry->CreateEntity();
                        projectile.AddComponent<RidigBodyComponent>();
                        auto& velocity1 = projectile.GetComponent<RidigBodyComponent>().velocity;
                        projectileVelocityArcGap(originVelocity, rotationDegrees, (realgap/2) + realgap*i*first, velocity1);
                        projectile.AddComponent<SpriteComponent>(PEC.spriteassetId, PEC.spritewidth, PEC.spriteheight, PEC.spritesrcRect, PEC.spritezIndex, PEC.spriteisFixed, PEC.spritediagonalSprite);
                        projectile.AddComponent<BoxColliderComponent>(PEC.boxwidth, PEC.boxheight, PEC.boxoffset);
                        projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(projectilescale,projectilescale), (rotationDegrees + realgap/2) + realgap*i*first); //init pos is same as emitter 
                        projectile.AddComponent<ProjectileComponent>(damage, duration, piercing, entity, parentGroupEnumInt, PEC.spriteOfParent, PEC.inflictsStatusEffect, PEC.statusEffect, PEC.durationMS, PEC.ignoresDefense);
                        if(PEC.oscillation){
                            projectile.AddComponent<OscillatingProjectileComponent>(PEC.amplitude, PEC.frequency, projectilePosition, false);
                        } else {
                            projectile.AddComponent<LinearProjectileComponent>();
                        }
                        projectile.Group(PROJECTILE);
                        numShotsEmitted++;

                        projectile = registry->CreateEntity();
                        projectile.AddComponent<RidigBodyComponent>();
                        auto& velocity2 = projectile.GetComponent<RidigBodyComponent>().velocity;
                        projectileVelocityArcGap(originVelocity, rotationDegrees, (realgap/-2) + realgap*-i*first, velocity2);
                        projectile.AddComponent<SpriteComponent>(PEC.spriteassetId, PEC.spritewidth, PEC.spriteheight, PEC.spritesrcRect, PEC.spritezIndex, PEC.spriteisFixed, PEC.spritediagonalSprite);
                        projectile.AddComponent<BoxColliderComponent>(PEC.boxwidth, PEC.boxheight, PEC.boxoffset);
                        projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(projectilescale,projectilescale), (rotationDegrees - realgap/2) + realgap*-i*first); //init pos is same as emitter 
                        projectile.AddComponent<ProjectileComponent>(damage, duration, piercing, entity, parentGroupEnumInt, PEC.spriteOfParent, PEC.inflictsStatusEffect, PEC.statusEffect, PEC.durationMS, PEC.ignoresDefense);
                        if(PEC.oscillation){
                            projectile.AddComponent<OscillatingProjectileComponent>(PEC.amplitude, PEC.frequency, projectilePosition, true);
                        } else {
                            projectile.AddComponent<LinearProjectileComponent>();
                        }
                        projectile.Group(PROJECTILE);
                        numShotsEmitted++;
                    }

                }
                
                if(IsPlayer){ // in any case...
                    const auto& classname = entity.GetComponent<ClassNameComponent>().classname;
                    assetStore->PlaySound(playerSounds[classname]);
                }
                PEC.lastEmissionTime = SDL_GetTicks();
                
            }
        }    
        
    }
}