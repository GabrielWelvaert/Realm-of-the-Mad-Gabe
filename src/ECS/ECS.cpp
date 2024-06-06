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

void System::AddEntityToSystem(Entity entity){
    // auto before = entities.data();
    entities.push_back(entity);
    // if(before != entities.data()){
    //     std::cout << "reallocation detected" << '\n';
    // }
    // if(entities.size() == 64){
    //     std::cout << "entities vector has size 64" << '\n';
    // }
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
    entitiesToBeAdded.clear();
    for(auto& system: systems){
        for(auto& entity: system.second->GetSystemEntities()){
            entity.Kill();
        }
    }
    Update();
}

Entity Registry::CreateEntity(){
    creationId++;
    int entityId;
    if(freeIds.empty()){
        entityId = numEntities++;
        if (entityId >= entityComponentSignatures.size()){
            entityComponentSignatures.resize(entityId+1);
        }
    } else {
        entityId = freeIds.front();
        freeIds.pop_front();
    }

    Entity entity(entityId);
    entity.registry = this; //assigning this entity to this registry! 
    entitiesToBeAdded.insert(entity);
    // if(entitiesToBeAdded.find(entity) == entitiesToBeAdded.end()){
    //     std::cout << entity.GetId() << " failed to enter entitiesToBeAdded!" << '\n';
    // }
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

    for (auto & system: systems){
        const auto& systemComponentSignature = system.second->GetComponentSignature();
        bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;
        if (isInterested){
            // if(entity.GetId() == 0)std::cout << "adding entity " << entity.GetId() << " to system " << std::endl;
            system.second->AddEntityToSystem(entity);
        }
    }
}

void Registry::RemoveEntityFromSystems(Entity entity){
    for (auto system: systems){
        // system.second->RemoveEntityFromSystem(entity);
        const Signature& systemSignature = system.second->GetComponentSignature();
        if((systemSignature & entity.getComponentSignature()) == systemSignature){
            // if(entity.GetId()==0){
            //     std::cout << entity.GetId() << " being removed from a system" << std::endl;
            //     if(entity.HasComponent<BoxColliderComponent>()){
            //         std::cout << "entity 0 has box collider" << std::endl;
            //     }
            // }
            system.second->RemoveEntityFromSystem(entity);
        }
    }
}

void Registry::Update() {
    //todo: add entites that are waiting to be created
    for (auto entity: entitiesToBeAdded){
        AddEntityToSystems(entity);
    }
    entitiesToBeAdded.clear();
    //todo: remove entities that are waiting to be killed
    for (auto entity: entitiesToBeKilled){
        // std::cout << entity.GetId() << " being processed for removal in registry->Update"<<std::endl;
        auto entityId = entity.GetId();
        RemoveEntityFromSystems(entity);
        entityComponentSignatures[entityId].reset();
        freeIds.push_back(entityId);

        for(auto pool: componentPools){
            if(pool){ // edge case where last entity 
                pool->RemoveEntityFromPool(entityId);
                // pool->printSize();
            }
        }

        // std::cout << entityId << " has died" << std::endl;
        entityIdToCreationId[entityId] = 0; // creationId of 0 used for dead entities
        // std::cout << entityIdToCreationId.at(entityId) << " this should be 0" << std::endl;
        // RemoveEntityTag(entity);
        RemoveEntityGroup(entity);
    }


    entitiesToBeKilled.clear();
}

// void Registry::TagEntity(Entity entity, const tags& tag){
//     entityPerTag.emplace(tag, entity);
//     tagPerEntity.emplace(entity.GetId(), tag);
// }

// bool Registry::EntityHasTag(Entity entity, const tags& tag) const {
//     if (tagPerEntity.find(entity.GetId()) == tagPerEntity.end()){
//         return false;
//     }
//     return entityPerTag.find(tag)->second == entity;
// }

// bool Registry::EntityHasTag(int id, const tags& tag) const {
//     if (tagPerEntity.find(id) == tagPerEntity.end()){
//         return false;
//     }
//     return entityPerTag.find(tag)->second.GetId() == id;
// }

// Entity Registry::GetEntityByTag(const tags& tag) const {
//     return entityPerTag.at(tag);
// }

// void Registry::RemoveEntityTag(Entity entity){
//     auto taggedEntity = tagPerEntity.find(entity.GetId());
//     if (taggedEntity != tagPerEntity.end()){
//         auto tag = taggedEntity->second;
//         entityPerTag.erase(tag);
//         tagPerEntity.erase(taggedEntity);
//     }
// }

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
