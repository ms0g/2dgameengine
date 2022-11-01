#pragma once

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <deque>
#include "../Logger/Logger.h"

static constexpr size_t MAX_COMPONENTS = 32;
/**
 * Signature
 *
 * @brief We use a bitset (1s and 0s) to keep track of which components an entity has,
 * and also helps keep track of which entities a system is interested in.
 */
using Signature = std::bitset<MAX_COMPONENTS>;

/**
 * Component
 * @brief
 */
class BaseComponent {
protected:
    static int nextID;
};

template<typename T>
class Component : public BaseComponent {
public:
    static int GetID() {
        static auto id = nextID++;
        return id;
    }

};

/**
 * Entity
 * @brief
 */

class Entity {
public:
    explicit Entity(size_t id) : m_id(id) {}

    Entity(const Entity& other) = default;

    [[nodiscard]] size_t GetID() const;

    // Manage entity tags and groups
    void Tag(const std::string& tag);

    [[nodiscard]] bool HasTag(const std::string& tag) const;

    void Group(const std::string& group);

    [[nodiscard]] bool BelongsToGroup(const std::string& group) const;

    void Kill();

    class Registry* registry{};

    template<typename T, typename ...Args>
    void AddComponent(Args&& ...args);

    template<typename T>
    void RemoveComponent();

    template<typename T>
    [[nodiscard]] bool HasComponent() const;

    template<typename T>
    T& GetComponent() const;

private:
    size_t m_id;

public:
    Entity& operator=(const Entity& other) = default;

    bool operator==(const Entity& other) const {
        return m_id == other.m_id;
    }

    bool operator!=(const Entity& other) const {
        return m_id != other.m_id;
    }

    bool operator<(const Entity& other) const {
        return m_id < other.m_id;
    }

    bool operator>(const Entity& other) const {
        return m_id > other.m_id;
    }
};

/**
 * System
 * @brief The system processes entities that contain a spesific signature
 */

class System {
public:
    System() = default;

    virtual ~System() = default;

    void AddEntityToSystem(Entity entity);

    void RemoveEntityToSystem(Entity entity);

    [[nodiscard]] std::vector<Entity> GetSystemEntities() const;

    [[nodiscard]] const Signature& GetComponentSignature() const;

    /**
     * @brief Defines the component type that entities must have to be considered by the system
     * @tparam T Component type
     */
    template<typename T>
    void RequireComponent();

private:
    Signature componentSignature;
    std::vector<Entity> entities;


};

template<typename T>
void System::RequireComponent() {
    const auto componentID = Component<T>::GetID();
    componentSignature.set(componentID);
}

/**
 * Pool
 * @brief A pool is just a vector of objects of type T
 */

class BasePool {
public:
    virtual ~BasePool() = default;
    virtual void RemoveEntityFromPool(size_t entitID) = 0;
};

template<typename T>
class Pool : public BasePool {
public:
    explicit Pool(size_t capacity = 100) {
        size = 0;
        data.resize(capacity);
    }

    ~Pool() override = default;

    [[nodiscard]] bool IsEmpty() const {
        return size == 0;
    }

    [[nodiscard]] size_t GetSize() const {
        return size;
    }

    void Resize(size_t n) {
        data.resize(n);
    }

    void Clear() {
        data.clear();
        entityIDtoIndex.clear();
        indextoEntityID.clear();
        size = 0;
    }

    void Add(T object) {
        data.emplace_back(object);
    }

    void Set(size_t entityID, T object) {
        if (entityIDtoIndex.find(entityID) != entityIDtoIndex.end()) {
            auto index = entityIDtoIndex[entityID];
            data[index] = object;
        } else {
            // When adding a new object, keep track of the id and its vector index
            auto index = size;
            entityIDtoIndex.emplace(entityID, index);
            indextoEntityID.emplace(index, entityID);

            if (index >= data.capacity())
                data.resize(size * 2);

            data[index] = object;
            size++;
        }
    }

    void Remove(size_t entityID) {
        // Copy the last element to the deleted position to keep the array packed
        auto indexOfRemoved = entityIDtoIndex[entityID];
        auto indexOfLast = size - 1;
        data[indexOfRemoved] = data[indexOfLast];

        // Update the index-entity maps to point to the correct elements
        auto entityIDOfLastElement = indextoEntityID[indexOfLast];
        entityIDtoIndex[entityIDOfLastElement] = indexOfRemoved;
        indextoEntityID[indexOfRemoved] = entityIDOfLastElement;

        entityIDtoIndex.erase(entityID);
        indextoEntityID.erase(indexOfLast);
        size--;
    }

    void RemoveEntityFromPool(size_t entitID) override {
        if (entityIDtoIndex.find(entitID) != entityIDtoIndex.end()) {
            Remove(entitID);
        }
    }

    T& Get(size_t entityID) {
        auto index = entityIDtoIndex[entityID];
        return static_cast<T&>(data[index]);
    }

    T& operator[](size_t entityID) {
        auto index = entityIDtoIndex[entityID];
        return data[index];
    }

private:
    std::vector<T> data;
    size_t size{};

    std::unordered_map<size_t, size_t> entityIDtoIndex;
    std::unordered_map<size_t, size_t> indextoEntityID;

};

/**
 * Registry
 * @brief The registry manages the creation and destruction of entities, add systems,
 * and components
 */

class Registry {
public:
    Registry() {
        Logger::Log("Registry ctor called");
    }

    ~Registry() {
        Logger::Log("Registry dtor called");
    }

    // Entity Management
    Entity CreateEntity();

    void KillEntity(Entity entity);

    // System Management
    void AddEntityToSystems(Entity entity);

    void RemoveEntityFromSystems(Entity entity);

    void Update();

    // Tag Management
    void TagEntity(Entity entity, const std::string& tag);

    bool EntityHasTag(Entity entity, const std::string& tag);

    Entity GetEntitybyTag(const std::string& tag);

    void RemoveEntityTag(Entity entity);

    // Group Management
    void GroupEntity(Entity entity, const std::string& group);

    [[nodiscard]] bool EntityBelongsToGroup(Entity entity, const std::string& group) const;

    [[nodiscard]] std::vector<Entity> GetEntitiesByGroup(const std::string& group) const;

    void RemoveEntityGroup(Entity entity);

    /********************************************
     *          Component Management            *
     ********************************************/

    /**
     * @brief Add a new component
     *
     * @tparam T Component type
     * @tparam Args Component param type
     * @param entity Entity
     * @param args Component params
     */
    template<typename T, typename ...Args>
    void AddComponent(Entity entity, Args&& ...args);

    /**
     * @brief Disable a component
     *
     * @tparam T Component type
     * @param entity Entity
     */
    template<typename T>
    void RemoveComponent(Entity entity);

    /**
     * @brief Check that a component is active
     * @tparam T
     * @param entity
     * @return
     */
    template<typename T>
    [[nodiscard]] bool HasComponent(Entity entity) const;

    template<typename T>
    T& GetComponent(Entity entity) const;

    /****************************************
     *          System Management           *
     ****************************************/

    /**
     * @brief
     * @tparam T
     * @tparam Args
     * @param args
     */
    template<typename T, typename ...Args>
    void AddSystem(Args&& ...args);

    /**
     * @brief
     * @tparam T
     */
    template<typename T>
    void RemoveSystem();

    /**
     * @brief
     * @tparam T
     * @return
     */
    template<typename T>
    [[nodiscard]] bool HasSystem() const;

    /**
     * @brief
     * @tparam T
     * @return
     */
    template<typename T>
    T& GetSystem() const;

private:
    size_t numEntities{0};
    /**
     * @brief Vector of component pools, each pool contains all the data of a certain component type
     * Vector index = Component type id
     * Pool index = entity id
     */
    std::vector<std::shared_ptr<BasePool>> componentPools;
    /**
     * @brief Vector of component signatures.
     * The signature lets us know which components are turned ON for a given entity
     * Vector index = entity id
     */
    std::vector<Signature> entityComponentSignatures;
    /**
     * @brief Map of active systems
     * Index = system type id
     */
    std::unordered_map<std::type_index, std::shared_ptr<System>> systems;
    /**
     * @brief Set of entities that are flagged to be added or removed
     * in the next registry update.
     */
    std::set<Entity> entitiesToBeAdded;
    std::set<Entity> entitiesToBeKilled;
    /**
     * @brief Entity Tags.
     * One tag name per entity
     */
    std::unordered_map<std::string, Entity> entityPerTag;
    std::unordered_map<size_t, std::string> tagPerEntity;
    /**
     * @brief Entity Groups
     * A set of entities per group name
     */
    std::unordered_map<std::string, std::set<Entity>> entitiesPerGroup;
    std::unordered_map<size_t, std::string> groupPerEntity;
    /**
     * @brief List of free IDs
     */
    std::deque<size_t> freeIDs;
};

template<typename T, typename... Args>
void Registry::AddComponent(Entity entity, Args&& ... args) {
    const auto componentID = Component<T>::GetID();
    const auto entityID = entity.GetID();

    // If the component id is greater than the current size of the component pool, resize it
    if (componentID >= componentPools.size()) {
        componentPools.resize(componentID + 1, nullptr);
    }

    // If we still don't have a pool for this component type
    if (!componentPools[componentID]) {
        componentPools[componentID] = std::make_shared<Pool<T>>();
    }

    // Get the pool of component values for that component type
    std::shared_ptr<Pool<T>> componentPool = std::static_pointer_cast<Pool<T>>(componentPools[componentID]);

    // Create a new component
    T newComponent(std::forward<Args>(args)...);

    // Add the new component to the component pool list, using the entity id as index
    componentPool->Set(entityID, newComponent);

    // Finally change the component signature of entity and set the component id on the bitset to 1
    entityComponentSignatures[entityID].set(componentID);

    Logger::Log("Component id=" + std::to_string(componentID) +
                " has been added to entity id=" + std::to_string(entityID));
}

template<typename T>
void Registry::RemoveComponent(Entity entity) {
    const auto componentID = Component<T>::GetID();
    const auto entityID = entity.GetID();

    std::shared_ptr<Pool<T>> componentPool = std::static_pointer_cast<Pool<T>>(componentPools[componentID]);
    componentPool->Remove(entityID);

    // Set the component signature for that entity to false
    entityComponentSignatures[entityID].set(componentID, false);

    Logger::Log("Component id=" + std::to_string(componentID) +
                " has been removed from entity id=" + std::to_string(entityID));
}

template<typename T>
bool Registry::HasComponent(Entity entity) const {
    const auto componentID = Component<T>::GetID();
    const auto entityID = entity.GetID();

    return entityComponentSignatures[entityID].test(componentID);
}

template<typename T>
T& Registry::GetComponent(Entity entity) const {
    const auto componentID = Component<T>::GetID();
    const auto entityID = entity.GetID();

    return std::static_pointer_cast<Pool<T>>(componentPools[componentID])->Get(entityID);
}

template<typename T, typename... Args>
void Registry::AddSystem(Args&& ... args) {
    systems.insert({std::type_index{typeid(T)}, std::make_shared<T>(std::forward<Args>(args)...)});
}

template<typename T>
void Registry::RemoveSystem() {
    systems.erase(std::type_index{typeid(T)});
}

template<typename T>
bool Registry::HasSystem() const {
    return systems.find(std::type_index{typeid(T)}) != systems.end();
}

template<typename T>
T& Registry::GetSystem() const {
    return *(std::static_pointer_cast<T>(systems.at(std::type_index{typeid(T)})));
}

///////////////////////////////////////////////////////////////
template<typename T, typename... Args>
void Entity::AddComponent(Args&& ... args) {
    registry->AddComponent<T>(*this, std::forward<Args>(args)...);
}

template<typename T>
void Entity::RemoveComponent() {
    registry->RemoveComponent<T>(*this);
}

template<typename T>
bool Entity::HasComponent() const {
    return registry->HasComponent<T>(*this);
}

template<typename T>
T& Entity::GetComponent() const {
    return registry->GetComponent<T>(*this);
}






