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
};

template<typename T>
class Pool : public BasePool {
public:
    explicit Pool(size_t size = 100) {
        data.reserve(size);
    }

    ~Pool() override = default;

    [[nodiscard]] bool IsEmpty() const {
        return data.empty();
    }

    [[nodiscard]] size_t GetSize() const {
        return data.size();
    }

    void Resize(size_t size) {
        data.resize(size);
    }

    void Clear() {
        data.clear();
    }

    void Add(T object) {
        data.emplace_back(object);
    }

    void Set(size_t index, T object) {
        data[index] = object;
    }

    T& Get(size_t index) {
        return static_cast<T&>(data[index]);
    }

    T& operator[](size_t index) {
        return data[index];
    }

private:
    std::vector<T> data;
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

    Entity CreateEntity();

    void KillEntity(Entity entity);

    void AddEntityToSystems(Entity entity);
    void RemoveEntityFromSystems(Entity entity);

    void Update();

    /** Component Management */

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

    /** System Management */
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

    // If the entity id is greater than the current size of the component pool, resize it
    if (entityID >= componentPool->GetSize()) {
        componentPool->Resize(numEntities);
    }

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
//////////////////////////////////////////////////////////////////
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





