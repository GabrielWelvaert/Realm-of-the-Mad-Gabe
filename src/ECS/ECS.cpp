#include "ECS.h"
#include <algorithm>
#include <iostream>
#include "../Utils/enums.h"

int IComponent::nextId = 0;

int Entity::GetId() const {
    return id;
}

unsigned int Entity::GetCreationId() const{
    return registry->GetCreationIdFromEntityId(this->GetId());
}

void Entity::Kill(){
    registry->KillEntity(*this);
}

// void Entity::Tag(const tags& tag){
//     registry->TagEntity(*this, tag);
// }

// bool Entity::HasTag(const tags& tag) const {
//     return registry->EntityHasTag(*this, tag);
// }

void Entity::Group(const groups& group){
    registry->GroupEntity(*this, group);
}

void Entity::monsterSubGroup(const monsterSubGroups& msg){
    registry->monsterSubGroupEntity(*this, msg);
}

bool Entity::BelongsToGroup(const groups& group) const {
    return registry->EntityBelongsToGroup(*this, group);
}

bool Entity::BelongsToMonsterSubGroup(const monsterSubGroups& group) const {
    return registry->EntityBelongsToMonsterSubGroup(*this, group);
}

void System::AddEntityToSystem(Entity entity){
    entities.push_back(entity);
}

void System::ForceResetEntities(){
    entities.clear();
}

void System::RemoveEntityFromSystem(Entity entity){
    entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
}

std::vector<Entity>& System::GetSystemEntities() {
    return entities;
}

const Signature& System::GetComponentSignature() const {
    return ComponentSignature;
}

void Registry::killAllEntities(){
    // entitiesToBeAdded.clear(); // data is already on pools. never do this unless doing a full reset
    Update(); 
    for(auto& system: systems){
        for(auto& entity: system.second->GetSystemEntities()){
            entity.Kill();
        }
    }
    Update();
}

Entity Registry::CreateEntity(){
    creationId++; // creationId is initialized as 0, so the first entity will have CID of 1
    int entityId;
    if(freeIds.empty()){
        entityId = numEntities++;
        if (entityId >= entityComponentSignatures.size()){
            entityComponentSignatures.resize(entityId+1);
        }
    } else {
        entityId = freeIds.front();
        freeIds.pop_front();
        std::unordered_set<int> dequeContents;
        for(auto& id: freeIds){
            if(dequeContents.find(id) != dequeContents.end()){
                std::cout << "duplicate " << id << " detected in CreateEntity" << '\n';
            }
            dequeContents.insert(id);
        }
    }

    Entity entity(entityId);
    entity.registry = this; //assigning this entity to this registry! 
    entitiesToBeAdded.insert(entity);
    entityIdToCreationId[entityId] = creationId;
    // std::cout << "entity created w/ ID " << entityId << std::endl;
    return entity;
}

void Registry::KillEntity(Entity entity){
    entitiesToBeKilled.insert(entity);
}

// add entity to each "entities" vector of each system 
void Registry::AddEntityToSystems(Entity entity){
    const auto entityId = entity.GetId();
    const auto entityComponentSignature = entityComponentSignatures[entityId];
    for(auto & system: systems){
        const auto& systemComponentSignature = system.second->GetComponentSignature();
        bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;
        if(isInterested){
            system.second->AddEntityToSystem(entity);
        }
    }
}

void Registry::RemoveEntityFromSystems(Entity entity){
    for(auto& system: systems){
        // system.second->RemoveEntityFromSystem(entity);
        const Signature& systemSignature = system.second->GetComponentSignature();
        if((systemSignature & entity.getComponentSignature()) == systemSignature){
            system.second->RemoveEntityFromSystem(entity);
        }
    }
}

void Registry::Update() {
    // new entities are added to their systems. their component data is already in the pools
    for(auto& entity: entitiesToBeAdded){
        AddEntityToSystems(entity);
    }
    entitiesToBeAdded.clear();
    // dead entities will be removed from their systems and they will be removed from pools
    for(auto& entity: entitiesToBeKilled){
        auto entityId = entity.GetId();
        RemoveEntityFromSystems(entity);
        entityComponentSignatures[entityId].reset();
        freeIds.push_back(entityId);

        // std::unordered_set<int> dequeContents;
        // for(auto& id: freeIds){
        //     if(dequeContents.find(id) != dequeContents.end()){
        //         std::cout << "killing " << entityId << ". ";
        //         std::cout << id << " was duplicate in freeIds..." << '\n';
        //     }
        //     dequeContents.insert(id);
        // }

        // if(freeIds.size() > numEntities){
        //     std::cout << "death of this entity made freeIds too large: " << entityId << " from group " << IdToGroup(entityId) << '\n';
        //     std::unordered_set<int> dequeContents;
        //     for(auto& id: freeIds){
        //         if(dequeContents.find(id) != dequeContents.end()){
        //             std::cout << id << " was duplicate in freeIds..." << '\n';
        //         }
        //         dequeContents.insert(id);
        //         if(!(id < numEntities)){
        //             std::cout << id << " was found in freeIds but it should have never been assigned to an entity! " << '\n';
        //         }
        //     }
        // }
        for(auto pool: componentPools){
            if(pool){ 
                pool->RemoveEntityFromPool(entityId);
            }
        }
        entityIdToCreationId[entityId] = 0; // creationId of 0 used for dead entities
        RemoveEntityGroup(entity);
    }


    entitiesToBeKilled.clear();
}

void Registry::HardReset(){
    // this function assumes killAllEntities() was just called and possibly failed to do its job
    
    // necessary things to "HardReset" pools
    for(auto itr = componentPools.begin(); itr != componentPools.end(); itr++){ 
        // restting size, entityIdToindex, and indexToId. pools not cleared as to not deconstruct any std::container data() ptrs:
        if(itr->get()){
            itr->get()->Clear();    
        }
    }
    
    // necessary things to "HardReset" registry
    entitiesToBeAdded.clear();
    entitiesToBeKilled.clear();
    freeIds.clear(); // clean freeIds because it could contain erroneous entities
    for(int i = 0; i < numEntities; i++){ // ensure freeIds only contains valid EntityIds;
        freeIds.push_back(i);
    }
    for(auto itr = entitiesPerGroup.begin(); itr != entitiesPerGroup.end(); itr++){
        itr->second.clear();
    }
    entitiesPerGroup.clear();
    groupPerEntity.clear();
    for(auto itr = entitiesPerMonsterSubGroup.begin(); itr != entitiesPerMonsterSubGroup.end(); itr++){
        itr->second.clear();
    }
    entitiesPerMonsterSubGroup.clear();
    monsterSubGroupPerEntity.clear();

    // necessary things to "HardReset" systems
    for(auto itr = systems.begin(); itr != systems.end(); itr++){ // clearing all entity vectors for each System
        if(itr->second.get()){
            itr->second.get()->ForceResetEntities();    
        }
    }

}

void Registry::monsterSubGroupEntity(Entity entity, const monsterSubGroups& msg){
    entitiesPerMonsterSubGroup.emplace(msg, std::unordered_set<int>());
    entitiesPerMonsterSubGroup[msg].emplace(entity.GetId());
    monsterSubGroupPerEntity.emplace(entity.GetId(), msg);
}

void Registry::GroupEntity(Entity entity, const groups& group){
    entitiesPerGroup.emplace(group, std::unordered_set<int>());
    // emplace automatically checks if value exists at key yet
    entitiesPerGroup[group].emplace(entity.GetId());
    groupPerEntity.emplace(entity.GetId(), group);
}

bool Registry::EntityBelongsToGroup(Entity entity, const groups& group) const {
    if (groupPerEntity.find(entity.GetId()) == groupPerEntity.end()){
        return false;
    } 
    return groupPerEntity.find(entity.GetId())->second == group;
}

bool Registry::IdBelongsToGroup(int Id, const groups& group) const {
    if (groupPerEntity.find(Id) == groupPerEntity.end()){
        return false;
    } 
    return groupPerEntity.find(Id)->second == group;
}

bool Registry::EntityBelongsToMonsterSubGroup(Entity entity, const monsterSubGroups& group) const {
    if(monsterSubGroupPerEntity.find(entity.GetId()) == monsterSubGroupPerEntity.end()){
        return false;
    }
    return monsterSubGroupPerEntity.find(entity.GetId())->second == group;
}

std::unordered_set<int>& Registry::allEntitesFromMonsterSubGroup(monsterSubGroups g){
    return entitiesPerMonsterSubGroup.at(g);
}

// std::vector<Entity> Registry::GetEntitiesByGroup(const groups& group) const {
//     auto& setOfEntities = entitiesPerGroup.at(group);
//     return std::vector<Entity>(setOfEntities.begin(), setOfEntities.end());
// }

// remove entity from the set of ids
void Registry::RemoveEntityGroup(Entity entity){
    auto id = entity.GetId();
    auto groupedEntity = groupPerEntity.find(id);
    if(groupedEntity != groupPerEntity.end()){
        auto group = entitiesPerGroup.find(groupedEntity->second);
        if(group != entitiesPerGroup.end()){
            auto entityInGroup = group->second.find(id);
            if(entityInGroup != group->second.end()){
                group->second.erase(entityInGroup);
                if(group->first == MONSTER){
                    removeEntityMonsterSubGroup(entity);
                }
            
            }
        }
        groupPerEntity.erase(groupedEntity);
    }
}

void Registry::removeEntityMonsterSubGroup(Entity entity){
    auto id = entity.GetId();
    auto groupedEntity = monsterSubGroupPerEntity.find(id);
    if(groupedEntity != monsterSubGroupPerEntity.end()){
        auto msg = entitiesPerMonsterSubGroup.find(groupedEntity->second);
        if(msg != entitiesPerMonsterSubGroup.end()){
            auto entityInMSG = msg->second.find(id);
            if(entityInMSG != msg->second.end()){
                msg->second.erase(entityInMSG);
            }
        }
        monsterSubGroupPerEntity.erase(groupedEntity); 
    }
}

int Registry::numEntitiesPerMonsterSubGroup(const monsterSubGroups& msg){
    if(entitiesPerMonsterSubGroup.find(msg) == entitiesPerMonsterSubGroup.end() || entitiesPerMonsterSubGroup.at(msg).size() == 0){
        return 0;
    }
    return entitiesPerMonsterSubGroup.at(msg).size();
}

unsigned int Registry::GetCreationIdFromEntityId(unsigned int Id) const {
    return entityIdToCreationId.at(Id);
}

const Signature& Entity::getComponentSignature() const{
    return registry->getComponentSignatureOfEntity(id);
}
