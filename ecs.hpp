#pragma once

#include <unordered_map>
#include <typeindex>
#include <vector>
#include <iostream>

#include "entity.hpp"
#include "component.hpp"

template <typename T>
class ComponentHandle {
    public:
        ComponentHandle(uint32_t id = (uint32_t) -1);

        uint32_t id() const;
        T* operator->();

    private:
        uint32_t id_;
};

class Entity;

class EntityId {
    public:
        EntityId(uint32_t _id = 0) : id(_id) {}

        bool operator==(const EntityId& eid) const {
            return id == eid.id;
        }

        friend std::ostream& operator<<(std::ostream& out, const EntityId& eid) {
            return out << eid.id;
        }

        inline Entity* operator->();

        uint32_t id;
    private:
        static EntityId nextID();
};

typedef struct Component {
    EntityId entity;
} Component;

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
        explicit Entity(EntityId id) : id_(id) {}

        EntityId getId() const { return id_; }

        template <typename T>
        ComponentHandle<T> getComponent();

    private:
        std::unordered_map<std::type_index, uint32_t> components_;
        EntityId id_;
};

namespace ECS {
    extern std::unordered_map<std::type_index, std::vector<uint8_t>> components_;
    extern std::unordered_map<std::type_index, std::unordered_map<EntityId, uint8_t>> newComponents_;
    extern std::unordered_map<EntityId, Entity> entities_;

    bool init();
    void shutdown();
    void update();

    EntityId createEntity();
    Entity* getEntity(EntityId id);
    void removeEntity(EntityId id);
    bool entityIsAlive(EntityId id);

    template <typename T, typename ...Args>
    ComponentHandle<T> createComponent(EntityId eid, Args&& ...args) {
        static_assert(std::is_base_of<Component, T>::value, "Error: Can only create components that inherit from class Component!");
        auto& arr = components_[typeid(T)];
        size_t currSize = arr.size();
        arr.resize(currSize + sizeof(T));

        uint32_t index = currSize / sizeof(T);
        ComponentHandle<T> ret(index);
    	auto obj = new(&arr[currSize]) T(std::forward<Args>(args)...);
        obj->entity = eid;

        return ret;
    }

    template <typename T>
    T* getComponent(const ComponentHandle<T>& handle) {
        return (T*) &components_[typeid(T)][handle.id() * sizeof(T)];
    }

    // TODO: make component list iterator
    template <typename T>
    T* getComponents(size_t& size) {
        size = components_[typeid(T)].size() / sizeof(T);
        return (T*) &components_[typeid(T)][0];
    }

}

template<typename T>
ComponentHandle<T>::ComponentHandle(uint32_t id) : id_(id) {}

template<typename T>
uint32_t ComponentHandle<T>::id() const { return id_; }

template<typename T>
T* ComponentHandle<T>::operator->() {
    return ECS::getComponent<T>(*this);
}

Entity* EntityId::operator->() {
    return ECS::getEntity(*this);
}

template <typename T>
ComponentHandle<T> Entity::getComponent() {
    auto it = components_.find(typeid(T));
    if (it != components_.end())
        return ComponentHandle<T>(it->second);
    else
        return ComponentHandle<T>();
}
