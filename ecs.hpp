#pragma once

#include "entity.hpp"
#include <unordered_map>
#include <typeindex>
#include <vector>
#include "component.hpp"
#include <iostream>

template <typename T>
class ComponentHandle {
    public:
        ComponentHandle(uint32_t id);

        uint32_t id() const;
        T* operator->();

    private:
        uint32_t id_;
        T* type_;
};

namespace ECS {
    extern std::unordered_map<std::type_index, std::vector<uint8_t>> components_;
    extern std::unordered_map<EntityId, Entity> entities_;

    bool init();
    void shutdown();
    void update();

    EntityId createEntity();
    void removeEntity(EntityId id);
    bool entityIsAlive(EntityId id);

    template <typename T, typename ...Args>
    ComponentHandle<T> createComponent(Args&& ...args) {
        auto& arr = components_[typeid(T)];
        size_t currSize = arr.size();
        arr.resize(currSize + sizeof(T));

        uint32_t index = currSize / sizeof(T);
        ComponentHandle<T> ret(index);
    	new(&arr[currSize]) T(std::forward(args)...);

        return ret;
    }

    template <typename T>
    T* getComponent(const ComponentHandle<T>& handle) {
        return (T*) &components_[typeid(T)][handle.id() * sizeof(T)];
    }

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
