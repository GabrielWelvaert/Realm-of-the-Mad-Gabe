#ifndef ECS_H
#define ECS_H

#include "../Utils/enums.h"
#include <bitset>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <set>
#include <memory>
#include <iostream>
#include <deque>
#include <cmath>
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include <type_traits>


template<typename T>
struct is_specific_struct : std::false_type {};

template<>
struct is_specific_struct<SpriteComponent> : std::true_type {};

// template<>
// struct is_specific_struct<TransformComponent> : std::true_type {};

const unsigned int MAX_COMPONENTS = 64; //max componenets for an entity
typedef std::bitset<MAX_COMPONENTS> Signature; //32 0's or 1's representing component signature for an entity/system

struct IComponent{
    protected:
        static int nextId; //this ID represents index in each entity's componentsignature
};

template <typename T> // template always associated with a component type, thats why its a templte
class Component: public IComponent { //inherit from struct 
public:
    static int GetId() { // we want one ID per component type; static methods belong to class rather than an instance!
        static auto id = nextId++; //static variables retain their value across multiple function calls
        return id;
    }
};

class Entity {
    private:
        int id;
    public:
        Entity() = default;
        Entity(int id): id(id) {};
        Entity(const Entity& entity) = default; //constructor that takes an entity as a parameter and uses = 
        int GetId() const;
        unsigned int GetCreationId() const;
        void Kill();

        Entity& operator =(const Entity& other) = default;
        bool operator ==(const Entity& other) const { return id == other.GetId();};
        bool operator !=(const Entity& other) const { return id != other.GetId();};
        bool operator <(const Entity& other) const { return id < other.GetId();};
        bool operator >(const Entity& other) const { return id > other.GetId();};
        
        template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
        template <typename TComponent> void RemoveComponent();
        template <typename TComponent> bool HasComponent() const;
        template <typename TComponent> TComponent& GetComponent() const; 

        class Registry* registry; //modern forward declaration
        const Signature& getComponentSignature() const;

        //tag and group management 
        // void Tag(const tags& tag);
        // bool HasTag(const tags& tag) const;
        void Group(const groups& group);
        bool BelongsToGroup(const groups& group) const;

        // monster sub grouping
        void monsterSubGroup(const monsterSubGroups& msg);
        

};

// abstract class can be used when you dont want to specify class
// NEEDED in registry when we declare our vector of componentPools since Pool is template class
class IPool {
    public:
        virtual ~IPool() = default;
        virtual void RemoveEntityFromPool(int entityId) = 0;
        virtual void printSize() = 0;
        virtual void Clear() = 0;
};

template <typename T>
class Pool: public IPool { //pool of component, where each index represents entity ID 
    private:
        std::vector<T> data; 
        unsigned int size;
        std::unordered_map<int,int> entityIdToIndex;
        std::unordered_map<int,int> indexToEntityId;
    public:
        
        Pool(int capacity = 100){ 
            size = 0; 
            data.resize(capacity);
        }
        
        virtual ~Pool() = default;
        
        void printSize() override {/*std::cout << size << std::endl;*/}

        bool IsEmpty() const { return size==0; }

        int GetSize() const { return size; }

        void Resize(int n){ 
            data.resize(n); 
            // std::cout << "###################################### " << std::endl;    
        }

        void Clear() override { 
            data.clear();
            entityIdToIndex.clear();
            indexToEntityId.clear();
            size = 0;    
        }

        void Add(T object){
            data.push_back(object);
        }

        void Set(int entityId, T object){
            int index;
            if(entityIdToIndex.find(entityId) != entityIdToIndex.end()){ //already exists; replace existing data 
                index = entityIdToIndex[entityId];
                data[index] = object;
            } else {
                index = size;
                entityIdToIndex.emplace(entityId, index); 
                // if(indexToEntityId.find(entityId) != indexToEntityId.end()){std::cout << "key already exists in the map, triggering a rehashing operation \n";}
                indexToEntityId.emplace(entityIdToIndex[entityId], entityId); // here! 
                if(index >= data.capacity()){
                    data.resize(size * 2);
                }
                data[index] = object;
                size++;
            }
            // if constexpr (is_specific_struct<T>::value) {
            //     if(entityId == 27){
            //         std::cout << "entityIdToIndex[entityId] = " << entityIdToIndex[entityId] << '\n';
            //         std::cout << "setting entity "<< entityId << " at " << "position " << index << " in spritecomp pool" << '\n';
            //     }
            //     if(index == 11){
            //         std::cout << "spritecomp pos 11 modified via set" << '\n';
            //     }
            //     if(entityIdToIndex.find(27) == entityIdToIndex.end()){
            //         std::cout << "entity 27 no longer detected in entityIdToIndex" << '\n';
            //     }
            // }
        }

        void Remove(int entityId){
            int indexOfRemoved = entityIdToIndex[entityId];
            int indexOfLast = size-1;
            data[indexOfRemoved] = data[indexOfLast];
            int idOfLast = indexToEntityId[indexOfLast];
            entityIdToIndex[idOfLast] = indexOfRemoved;
            indexToEntityId[indexOfRemoved] = idOfLast;
            entityIdToIndex.erase(entityId);
            indexToEntityId.erase(indexOfLast);
            size--;
            // if(size > 4000000000){
            //     std::cout << "overflow of some pool! " << std::endl;
            // }
            // if constexpr (is_specific_struct<T>::value) {
            //     if(entityId == 27){
            //         std::cout << "removing " << entityId <<  " from position " << indexOfRemoved << " in spritecomp pool" << '\n';
            //     }
            //     if(indexOfRemoved == 11){
            //         std::cout << "removing position 11 in spritecomp pool" << '\n';
            //     }
            //     if(entityIdToIndex.find(27) == entityIdToIndex.end()){
            //         std::cout << "entity 27 no longer detected in entityIdToIndex" << '\n';
            //     }
            // }

        }

        void RemoveEntityFromPool(int entityId) override {
            if(entityIdToIndex.find(entityId) != entityIdToIndex.end()){
                Remove(entityId);
            }
        }

        T& Get(int entityId) { 
            if(entityIdToIndex.find(entityId) == entityIdToIndex.end()){
                std::cout << "entity " << entityId << " engine misuse detected (GetComponent<X> used on entity without Component X); use bt gdb command to see culprit" << std::endl;
                // for(auto& pair: entityIdToIndex){
                //     std::cout << pair.first << ", " << pair.second << '\n';
                // }
            }
            return static_cast<T&>(data[entityIdToIndex[entityId]]); 
        }

        T& operator [](unsigned int index){ 
            return data[index]; 
        }

};

// system parent class 
class System {
    private:
        Signature ComponentSignature; // system will only have an entity if signature matches
        std::vector<Entity> entities; //list of entities that a system is interested in
    public:
        System() = default;
        ~System() = default;
        void AddEntityToSystem(Entity entity);
        void RemoveEntityFromSystem(Entity entity);
        std::vector<Entity>& GetSystemEntities();
        const Signature& GetComponentSignature() const;
        template <typename TComponent> void RequireComponent(); // defines componenets needed by an entity for a system to be interested
};

template <typename TComponent>
void System::RequireComponent(){
    const auto componentId = Component<TComponent>::GetId();
    ComponentSignature.set(componentId); 
}

class Registry {
    private:
        unsigned int numEntities = 0; // used for entityId; recycled from dead entities 
        unsigned int creationId = 0; // reset creation values, NEVER recycled
        
        // unordered_map of entityId to CreationId to check if entity has died for ex
        std::unordered_map<unsigned int, unsigned int> entityIdToCreationId;  

        //set of entities to be added or removed in next registry update:
        std::set<Entity> entitiesToBeAdded;
        std::set<Entity> entitiesToBeKilled;
        std::deque<int> freeIds;

        //vector of component pools, wherein each pool contains all entities of a component
        //vector index = component type id, pool index = entity id
        std::vector<std::shared_ptr<IPool>> componentPools;

        //vector of component signatures (which are bitsets), each index is a specific entity's component signature 
        std::vector<Signature> entityComponentSignatures;

        //hashmap of active systems. system type_id = index
        // key = type_index, value = pointer to system
        // type_index is just a unqiue ID for an object
        std::unordered_map<std::type_index, std::shared_ptr<System>> systems; 

        // tags are unique identifiers for ONE entity (not used currently)
        // std::unordered_map<tags, Entity> entityPerTag; // unique tag name per entity; may not need
        // std::unordered_map<int, tags> tagPerEntity; // get unique entity tag from id

        // group is a way of identifying a GROUP of entities: ex projectile, wall
        // std::unordered_map<groups, std::set<Entity>> entitiesPerGroup; // group name returns all entities of group
        std::unordered_map<groups, std::unordered_set<int>> entitiesPerGroup; // group to hash set of ids
        std::unordered_map<int, groups> groupPerEntity; // id to group name 

        // secondary grouping is used to further categorize monsters, if necessary
        std::unordered_map<monsterSubGroups, std::unordered_set<int>> entitiesPerMonsterSubGroup;
        std::unordered_map<int, monsterSubGroups> monsterSubGroupPerEntity;

    public:
        Registry() = default;
        ~Registry() {
            // std::cout << "Registry destructor called" << std::endl;
        }
        void Update();
        Entity CreateEntity();
        void KillEntity(Entity entity);

        template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
        template <typename TComponent> void RemoveComponent(Entity entity);
        template <typename TComponent> bool HasComponent(Entity entity);
        template <typename TComponent> bool HasComponent(int id);
        template <typename TComponent> TComponent& GetComponent(Entity entity) const;
        template <typename TComponent> TComponent& GetComponent(int entityID) const;
        template <typename TComponent> void printPoolSize() const;

        //needs to be template since system type is generic 
        template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
        template <typename TSystem> void RemoveSystem();
        template <typename TSystem> bool HasSystem() const;
        template <typename TSystem> TSystem& GetSystem() const;

        void AddEntityToSystems(Entity entity);
        void RemoveEntityFromSystems(Entity entity);

        //tag management 
        // void TagEntity(Entity entity, const tags& tag);
        // bool EntityHasTag(Entity entity, const tags& tag) const;
        // bool EntityHasTag(int id, const tags& tag) const;
        // Entity GetEntityByTag(const tags& tag) const;
        // void RemoveEntityTag(Entity entity);
        // void printTagsAndGroups();

        //group mgmt 
        void GroupEntity(Entity entity, const groups& group);
        bool EntityBelongsToGroup(Entity entity, const groups& group) const;
        bool IdBelongsToGroup(int Id, const groups& group) const;
        // std::vector<Entity> GetEntitiesByGroup(const groups& group) const;
        void RemoveEntityGroup(Entity entity);
        inline groups IdToGroup(int id){return groupPerEntity.at(id);}

        void monsterSubGroupEntity(Entity entity, const monsterSubGroups& msg);
        void removeEntityMonsterSubGroup(Entity entity);
        int numEntitiesPerMonsterSubGroup(const monsterSubGroups& msg);

        bool entityWasKilledThisFrame(Entity& entity){
            return entitiesToBeKilled.find(entity) != entitiesToBeKilled.end();
        };

        unsigned int GetCreationIdFromEntityId(unsigned int Id) const;
        unsigned int getCurrentCreationId() const {return creationId;};
        unsigned int getNumberOfLivingEntities() const {
            // for(auto& pair: entityIdToCreationId){
            //     if(pair.second != 0){
            //         std::cout << pair.first << " was not killed!-- ";
            //         std::cout << entityComponentSignatures.at(pair.first) << '\n';
            //     }
            // }
            return numEntities - freeIds.size();
        };

        const Signature& getComponentSignatureOfEntity(unsigned int id) const {return entityComponentSignatures.at(id);};
        void printEntitiesToBeKilled() const {/*for(const auto& x: entitiesToBeKilled){std::cout << x.GetId() << '\n';}*/}
        void killAllEntities();
        void printFreeIds() const {/*for(const auto& x: freeIds){std::cout << x << std::endl;}*/}

        inline bool entityIsAlive(int id, unsigned int creationId){
            return GetCreationIdFromEntityId(id) == creationId;
        }

};

template <typename TSystem, typename ...TArgs>
void Registry::AddSystem(TArgs&& ...args) {
    std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem>
void Registry::RemoveSystem(){
    auto system = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(system);
}

template <typename TSystem>
bool Registry::HasSystem() const {
    return systems.find(std::type_index(typeid(TSystem))) != systems.end(); //if not there we hit end
}

template <typename TSystem>
TSystem& Registry::GetSystem() const {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(system->second)); //dereference the whole thing! 
}

template <typename TComponent>
void Registry::printPoolSize() const {
    const auto componentId = Component<TComponent>::GetId();
    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    componentPool->printSize();
}

template <typename TComponent, typename ...TArgs> // targs is like *args; we dont know number of parameters
void Registry::AddComponent(Entity entity, TArgs&& ...args){
    
    //get compnentid and entityid
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    // if componentid is greater than size of compnentpools, resize so we can include this component
    if (componentId >= componentPools.size()){
        componentPools.resize(componentId+1, nullptr);
    }

    // add pool for new component in componentPools
    if (!componentPools[componentId]){
        std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
        componentPools[componentId] = newComponentPool;
    }

    // get pointer to pool for this component
    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    // forward parameters to component constructor via Tcomponent
    TComponent newComponent(std::forward<TArgs>(args)...);

    // actually add the component to the pool
    componentPool->Set(entityId, newComponent);

    //update component signature for this entity in Regsistry's entityComponentSignature 
    entityComponentSignatures[entityId].set(componentId);

    // std::cout << "Component " << componentId << " was added to entity " << entityId << std::endl;

}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity){
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();
    entityComponentSignatures[entityId].set(componentId,false);
    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    componentPool->Remove(entityId);
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity){
    const auto componenetId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    return entityComponentSignatures[entityId].test(componenetId); //test is a method for Signature types
}

template <typename TComponent>
bool Registry::HasComponent(int id){
    const auto componenetId = Component<TComponent>::GetId();
    return entityComponentSignatures[id].test(componenetId); //test is a method for Signature types
}

template <typename TComponent>
TComponent& Registry::GetComponent(Entity entity) const {
    const auto componenetId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();
    auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componenetId]);
    return componentPool->Get(entityId);
}

template <typename TComponent>
TComponent& Registry::GetComponent(int entityId) const {
    const auto componenetId = Component<TComponent>::GetId();
    auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componenetId]);
    return componentPool->Get(entityId);
}

template <typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args) {
    registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...); // "this" keyword holds address; must dereference to pass value!
}

template <typename TComponent>
void Entity::RemoveComponent() {
    registry->RemoveComponent<TComponent>(*this);
}


template <typename TComponent>
bool Entity::HasComponent() const {
    return registry->HasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::GetComponent() const {
    return registry->GetComponent<TComponent>(*this);
}

#endif