#include "component.hpp"

// template <typename T>
// ComponentHandle<T>::ComponentHandle(uint32_t id) : id_(id)
// {
// }


template <typename T>
T* ComponentHandle<T>::operator->() {
    return nullptr;
}
