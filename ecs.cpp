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
    // TODO: make private
    std::unordered_map<std::type_index, std::vector<uint8_t>> components_;
    std::unordered_map<std::type_index, std::unordered_map<EntityId, uint8_t>> newComponents_;
    std::unordered_map<EntityId, Entity> entities_;

    bool init() {
    }

    // TODO: destruct all objects
    void shutdown() {
    }

    void update() {
    }

    EntityId createEntity() {
        EntityId id = nextID();
        entities_.emplace(id, Entity(id));
        return id;
    }

    Entity* getEntity(EntityId id) {
        auto it = entities_.find(id);
        if (it != entities_.end())
            return &it->second;
        else
            return nullptr;
    }

    void removeEntity(EntityId id) {
        auto it = entities_.find(id);
        if (it != entities_.end())
            entities_.erase(id);
    }

    bool entityIsAlive(EntityId id) {
        return entities_.find(id) != entities_.end();
    }

} // namespace ECS
