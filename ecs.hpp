#pragma once

#include "entity.hpp"
#include <unordered_map>

class ECS {
    public:
        ECS() = default;
        ~ECS();

        bool init();
        void shutdown();
        void update();

        EntityId createEntity();
        void removeEntity(EntityId id);
        bool entityIsAlive(EntityId id) const;

        // void registerSystem(ComponentSystem* system);
        // void unregisterSystem(ComponentSystem* system);


    // private:
        std::unordered_map<EntityId, Entity> entities_;
        // std::vector<ComonentSystem*> systems_;

        // std::unordered_map<std::type_index, Component*
};
