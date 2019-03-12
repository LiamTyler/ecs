#pragma once

#include <stdint.h>
#include <functional>
#include <iostream>

class EntityId {
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


class Entity {
    public:
        explicit Entity(EntityId id);

        // template <typename CompType, typename ...Args>
        // CompType* addComponent(Args&&... args) {
        //     return nullptr;
        // }

        // std::unordered_map<std::type_index, ComponentHandle> components;

        EntityId getId() const { return id_; }

    private:
        EntityId id_;
};
