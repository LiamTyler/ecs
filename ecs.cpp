#include "ecs.hpp"
#include "component_system.hpp"
#include <algorithm>

ECS::~ECS() {
    shutdown();
}

bool ECS::init() {
    /*
    for (const auto& system : systems_)
        if (!system->init())
            return false;
    return true;
    */
}

void ECS::shutdown() {
    // for (const auto& system : systems_)
    //     delete system;
    // systems_.clear();

}

void ECS::update() {
    // for (const auto& system : systems_)
    //     system->update();
}

EntityId ECS::createEntity() {
    EntityId id = EntityId::nextID();
    entities_.emplace(id, Entity(id));
    return id;
}

void ECS::removeEntity(EntityId id) {
    auto it = entities_.find(id);
    if (it != entities_.end())
        entities_.erase(id);
}

bool ECS::entityIsAlive(EntityId id) const {
    return entities_.find(id) != entities_.end();
}
/*
void ECS::registerSystem(ComponentSystem* system) {
    systems_.push_back(system);
}

void ECS::unregisterSystem(ComponentSystem* system) {
    auto it = std::find(systems_.begin(), systems_.end(), system);
    if (it != systems_.end())
        systems_.erase(it);
}
*/
