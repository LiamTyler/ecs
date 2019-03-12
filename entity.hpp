#pragma once

#include <stdint.h>
#include <functional>
#include <iostream>

// using EntityId = uint32_t;

class EntityId {
    friend class ECS;
    public:
        EntityId(uint32_t id);

        bool operator==(const EntityId& eid) const;
        friend std::ostream& operator<<(std::ostream& out, const EntityId& eid);

        uint32_t id;
    private:
        static EntityId nextID();
};

namespace std {

  template <>
  struct hash<EntityId>
  {
    std::size_t operator()(const EntityId& eid) const
    {
      return std::hash<uint32_t>()(eid.id);
    }
  };

}

// class Component;

class Entity {
    friend class ECS;
    public:

        // template <typename CompType, typename ...Args>
        // CompType* addComponent(Args&&... args) {
        //     return nullptr;
        // }

        // std::unordered_map<std::type_index, Component
        //

        EntityId getId() const { return id_; }

    private:
        explicit Entity(EntityId id);
        static uint32_t nextID();
        EntityId id_;
};
