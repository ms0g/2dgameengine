#include "ECS.h"

int BaseComponent::nextID{0};

size_t Entity::GetID() const {
    return m_id;
}

void Entity::Kill() {
    registry->KillEntity(*this);
}

void Entity::Tag(const std::string& tag) {
    registry->TagEntity(*this, tag);
}

bool Entity::HasTag(const std::string& tag) const {
    return registry->EntityHasTag(*this, tag);
}

void Entity::Group(const std::string& group) {
    registry->GroupEntity(*this, group);
}

bool Entity::BelongsToGroup(const std::string& group) const {
    return registry->EntityBelongsToGroup(*this, group);
}

////////////////////////////
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

////////////////////////////////////////////
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
    // Processing the entities that are waiting to be created to the active Systems
    for (auto& entity: entitiesToBeAdded) {
        AddEntityToSystems(entity);
    }
    entitiesToBeAdded.clear();

    // Process the entities that are waiting to be killed from the active Systems
    for (auto& entity: entitiesToBeKilled) {
        RemoveEntityFromSystems(entity);
        entityComponentSignatures[entity.GetID()].reset();

        // Make the entity id available to be reused
        freeIDs.push_back(entity.GetID());

        // Remove any traces of that entity from the tag/group maps
        RemoveEntityTag(entity);
        RemoveEntityGroup(entity);
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

void Registry::TagEntity(Entity entity, const std::string& tag) {
    entityPerTag.emplace(tag, entity);
    tagPerEntity.emplace(entity.GetID(), tag);
}

bool Registry::EntityHasTag(Entity entity, const std::string& tag) {
    if (tagPerEntity.find(entity.GetID()) == tagPerEntity.end()) {
        return false;
    }
    return entityPerTag.find(tag)->second == entity;
}

Entity Registry::GetEntitybyTag(const std::string& tag) {
    return entityPerTag.at(tag);
}

void Registry::RemoveEntityTag(Entity entity) {
    auto taggedEntity = tagPerEntity.find(entity.GetID());
    if (taggedEntity != tagPerEntity.end()) {
        auto tag = taggedEntity->second;
        entityPerTag.erase(tag);
        tagPerEntity.erase(taggedEntity);
    }
}

void Registry::GroupEntity(Entity entity, const std::string& group) {
    entitiesPerGroup.emplace(group, std::set<Entity>{});
    entitiesPerGroup[group].emplace(entity);
    groupPerEntity.emplace(entity.GetID(), group);
}

bool Registry::EntityBelongsToGroup(Entity entity, const std::string& group) const {
    if (entitiesPerGroup.find(group) == entitiesPerGroup.end())
        return false;
    auto groupEntities = entitiesPerGroup.at(group);
    return groupEntities.find(entity) != groupEntities.end();
}

std::vector<Entity> Registry::GetEntitiesByGroup(const std::string& group) const {
    auto& setOfEntities = entitiesPerGroup.at(group);
    return {setOfEntities.begin(), setOfEntities.end()};
}

void Registry::RemoveEntityGroup(Entity entity) {
    // if in group, remove entity from group management
    auto groupedEntity = groupPerEntity.find(entity.GetID());
    if (groupedEntity != groupPerEntity.end()) {
        auto group = entitiesPerGroup.find(groupedEntity->second);
        if (group != entitiesPerGroup.end()) {
            auto entityInGroup = group->second.find(entity);
            if (entityInGroup != group->second.end()) {
                group->second.erase(entityInGroup);
            }
        }
        groupPerEntity.erase(groupedEntity);
    }
}


