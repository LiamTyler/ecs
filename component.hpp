#pragma once

#include <stdint.h>

// using ComponentHandle = uint32_t;

template <typename T>
class ComponentHandle {
    public:
        ComponentHandle(uint32_t id) : id_(id) {}
        // bool operator==(const ComponentHandle& compHandle) const;
        //

        uint32_t id() const { return id_; }
        // T* type() const { return ; }
        T* operator->();

    private:
        uint32_t id_;
        T* type_;
};

// class Component {
// 
// }
