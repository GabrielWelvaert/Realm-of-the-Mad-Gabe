#include "HealOtherSystem.h"

HealOtherSystem::HealOtherSystem(){
    RequireComponent<HealOtherComponent>();
    RequireComponent<TransformComponent>();
}

void HealOtherSystem::Update(std::unique_ptr<Registry>& registry, std::unique_ptr<Factory>& factory){
    auto time = SDL_GetTicks();
    for(auto& entity: GetSystemEntities()){
        auto& hoc = entity.GetComponent<HealOtherComponent>();
        if(!hoc.beneficiaryIsDead && time > hoc.timeOfLastHeal + hoc.interval){ // beneficiary already dead and its time
            if(registry->entityIsAlive(hoc.beneficiaryId, hoc.beneficiaryCreationId)){ // beneficiary is indeed stil alive
                auto& HPMPbeneficiary = registry->GetComponent<HPMPComponent>(hoc.beneficiaryId); 
                hoc.timeOfLastHeal = time;
                if(HPMPbeneficiary.activehp <= static_cast<double>(HPMPbeneficiary.maxhp)){ // if beneficiary doesnt have max hp
                    HPMPbeneficiary.activehp += hoc.amount;
                    int amountHealed = hoc.amount;
                    if(HPMPbeneficiary.activehp > HPMPbeneficiary.maxhp){
                        amountHealed = hoc.amount - (static_cast<int>(HPMPbeneficiary.activehp) - static_cast<int>(HPMPbeneficiary.maxhp));
                        if(amountHealed == 0){
                            continue;
                        }
                    }
                    const auto& transform = entity.GetComponent<TransformComponent>();
                    const auto& beneficiaryTransform = registry->GetComponent<TransformComponent>(hoc.beneficiaryId);
                    Entity healtext = registry->CreateEntity();
                    healtext.AddComponent<TextLabelComponent>("+" + std::to_string(amountHealed),"damagefont",xpgreen,false,350,hoc.beneficiaryId,hoc.beneficiaryCreationId);
                    healtext.AddComponent<TransformComponent>(beneficiaryTransform.position);
                    if(hoc.beneficiaryId != entity.GetId()){ // only spawn green partiles if beneficiary is not self!
                        factory->spawnHealOtherPartiles(registry,transform.center, beneficiaryTransform.center);    
                    }
                }

            } else { // beneficiary has perished
                hoc.beneficiaryIsDead = true;
                continue;
            }
        }
    }
}