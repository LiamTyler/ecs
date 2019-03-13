#pragma once

#include <unordered_map>
#include <typeindex>
#include <vector>
#include <iostream>
#include <deque>

#include "entity.hpp"
#include "component.hpp"

#include <memory>
#include <cxxabi.h>

inline std::string demangle(const char* name) {
    int status = -4; // some arbitrary value to eliminate the compiler warning

    // enable c++11 by passing the flag -std=c++11 to g++
    std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(name, NULL, NULL, &status),
        std::free
    };

    return (status==0) ? res.get() : name ;
}

template <class T>
inline std::string type(const T& t) {
    return demangle(typeid(t).name());
}


// The whole ids = (generation + index) idea from:
// http://bitsquid.blogspot.com/2014/08/building-data-oriented-entity-system.html
const uint32_t COMPONENT_INDEX_BITS = 24;
const uint32_t COMPONENT_INDEX_MASK = (1<<COMPONENT_INDEX_BITS)-1;

const uint32_t COMPONENT_GENERATION_BITS = 8;
const uint32_t COMPONENT_GENERATION_MASK = (1<<COMPONENT_GENERATION_BITS)-1;

const uint32_t MINIMUM_FREE_COMPONENTS = 8;

const uint32_t NULL_COMPONENT_ID = ~0;

template <typename T>
class ComponentHandle {
    public:
        ComponentHandle(uint32_t id = NULL_COMPONENT_ID);
        ComponentHandle(uint32_t index, uint32_t generation);

        uint32_t id() const { return id_; }
        uint32_t index() const { return id_ & COMPONENT_INDEX_MASK; }
        uint32_t generation() const { return (id_ >> COMPONENT_INDEX_BITS) & COMPONENT_GENERATION_MASK; }

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

        template <typename T, typename ...Args>
        ComponentHandle<T> createComponent(Args&& ...args);

        template <typename T>
        ComponentHandle<T> getComponent();

        friend std::ostream& operator<<(std::ostream& out, const Entity& e) {
            out << "id = " << e.id_ << ", num components = " << e.components_.size() << "\n";
            for (const auto& c : e.components_) {
                out << "Type = '" << demangle(c.first.name()) << "'\n";
            }
            return out;
        }

    private:
        std::unordered_map<std::type_index, uint32_t> components_;
        EntityId id_;
};

namespace ECS {
    extern std::unordered_map<std::type_index, std::vector<uint8_t>> components_;
    extern std::unordered_map<std::type_index, std::vector<uint32_t>> components_generation_;
    extern std::unordered_map<std::type_index, std::deque<uint32_t>> components_free_list_;
    // extern std::unordered_map<std::type_index, std::unordered_map<EntityId, uint8_t>> newComponents_;
    extern std::unordered_map<EntityId, Entity> entities_;

    bool init();
    void shutdown();
    void update();

    void printState();

    EntityId createEntity();
    Entity* getEntity(EntityId id);
    void removeEntity(EntityId id);
    bool entityIsAlive(EntityId id);

    template <typename T, typename ...Args>
    ComponentHandle<T> createComponent(EntityId eid, Args&& ...args) {
        // Make sure that T inherits from Component
        static_assert(std::is_base_of<Component, T>::value, "Error: Can only create components that inherit from class Component!");

        auto& components = components_[typeid(T)];
        auto& free_list  = components_free_list_[typeid(T)];
        auto& generation = components_generation_[typeid(T)];

        uint32_t idx;
        // see if there are enough free spaces
        if (free_list.size() > MINIMUM_FREE_COMPONENTS) {
            idx = free_list.front();
            free_list.pop_front();
        } else {
            generation.push_back(0);

            size_t currSize = components.size();
            components.resize(currSize + sizeof(T));
            idx = currSize / sizeof(T);
        }

        ComponentHandle<T> ret(idx, generation[idx]);
    	auto obj = new(&components[idx * sizeof(T)]) T(std::forward<Args>(args)...);
        obj->entity = eid;

        return ret;
    }

    template <typename T>
    void destroyComponent(const ComponentHandle<T>& handle) {
        auto& generation = components_generation_[typeid(T)];
        auto idx = handle.index();
        ++generation[idx];
        components_free_list_[typeid(T)].push_back(idx);
        // TODO: destructor?
        // components_[typeid(T)][
    }

    template <typename T>
    T* getComponent(const ComponentHandle<T>& handle) {
        return (T*) &components_[typeid(T)][handle.id() * sizeof(T)];
    }

    template <typename T>
    bool componentAlive(const ComponentHandle<T>& handle) {
        const auto& generation = components_generation_[typeid(T)];
        return generation[handle.index()] == handle.generation();
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
ComponentHandle<T>::ComponentHandle(uint32_t index, uint32_t generation) :
    id_(index + (generation << COMPONENT_INDEX_BITS))
{
}

template<typename T>
T* ComponentHandle<T>::operator->() {
    return ECS::getComponent<T>(*this);
}

Entity* EntityId::operator->() {
    return ECS::getEntity(*this);
}

template <typename T, typename ...Args>
ComponentHandle<T> Entity::createComponent(Args&& ...args) {
    auto it = components_.find(typeid(T));
    if (it == components_.end()) {
        auto handle = ECS::createComponent<T>(id_, std::forward<Args>(args)...);
        components_[typeid(T)] = handle.id();
        return handle;
    } else {
        return ComponentHandle<T>();
    }
}

template <typename T>
ComponentHandle<T> Entity::getComponent() {
    auto it = components_.find(typeid(T));
    if (it != components_.end())
        return ComponentHandle<T>(it->second);
    else
        return ComponentHandle<T>();
}
