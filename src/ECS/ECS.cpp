#include "ECS.h"

int BaseComponent::nextID{0};

size_t Entity::GetID() const {
    return m_id;
}

void Entity::Kill() {
    registry->KillEntity(*this);
}

void System::AddEntityToSystem(Entity entity) {
    entities.emplace_back(entity);

}

void System::RemoveEntityToSystem(Entity entity) {
    entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity other) {
        return entity == other;
    }), entities.cend());
}

std::vector<Entity> System::GetSystemEntities() const {
    return entities;
}

const Signature& System::GetComponentSignature() const {
    return componentSignature;
}

Entity Registry::CreateEntity() {
    size_t entityID;
    if (freeIDs.empty()) {
        entityID = numEntities++;
        if (entityID >= entityComponentSignatures.size()) {
            entityComponentSignatures.resize(entityID + 1);
        }
    } else {
        entityID = freeIDs.front();
        freeIDs.pop_front();
    }

    Entity entity{entityID};
    entity.registry = this;
    entitiesToBeAdded.insert(entity);

    Logger::Log("Entity created with ID = " + std::to_string(entityID));

    return entity;

}

void Registry::KillEntity(Entity entity) {
    entitiesToBeKilled.insert(entity);
}

void Registry::Update() {
    for (auto& entity: entitiesToBeAdded) {
        AddEntityToSystems(entity);
    }
    entitiesToBeAdded.clear();

    for (auto& entity: entitiesToBeKilled) {
        RemoveEntityFromSystems(entity);
        entityComponentSignatures[entity.GetID()].reset();

        freeIDs.push_back(entity.GetID());
    }
    entitiesToBeKilled.clear();


}

void Registry::AddEntityToSystems(Entity entity) {
    const auto entityID = entity.GetID();
    const auto& entityComponentSignature = entityComponentSignatures[entityID];

    for (auto& system: systems) {
        const auto& systemComponentSignature = system.second->GetComponentSignature();
        bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

        if (isInterested) {
            system.second->AddEntityToSystem(entity);
        }
    }
}

void Registry::RemoveEntityFromSystems(Entity entity) {
    for (auto& system: systems) {
        system.second->RemoveEntityToSystem(entity);
    }
}



