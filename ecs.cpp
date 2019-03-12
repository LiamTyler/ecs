#include "ecs.hpp"
#include "component_system.hpp"
#include <algorithm>

namespace {

    EntityId nextID() {
        static uint32_t id = 1;
        EntityId eid(id);
        id++;
        return eid;
    }

}

namespace ECS {
    std::unordered_map<std::type_index, std::vector<uint8_t>> components_;
    std::unordered_map<EntityId, Entity> entities_;

    bool init() {
        /*
        for (const auto& system : systems_)
            if (!system->init())
                return false;
        return true;
        */
    }

    void shutdown() {
        // for (const auto& system : systems_)
        //     delete system;
        // systems_.clear();

    }

    void update() {
        // for (const auto& system : systems_)
        //     system->update();
    }

    EntityId createEntity() {
        EntityId id = nextID();
        entities_.emplace(id, Entity(id));
        return id;
    }

    void removeEntity(EntityId id) {
        auto it = entities_.find(id);
        if (it != entities_.end())
            entities_.erase(id);
    }

    bool entityIsAlive(EntityId id) {
        return entities_.find(id) != entities_.end();
    }

    /*
    void registerSystem(ComponentSystem* system) {
        systems_.push_back(system);
    }

    void unregisterSystem(ComponentSystem* system) {
        auto it = std::find(systems_.begin(), systems_.end(), system);
        if (it != systems_.end())
            systems_.erase(it);
    }
    */

} // namespace ECS
