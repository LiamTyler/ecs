#include "entity.hpp"

EntityId::EntityId(uint32_t _id) : id(_id) {}

bool EntityId::operator==(const EntityId& eid) const {
    return id == eid.id;
}

std::ostream& operator<<(std::ostream& out, const EntityId& eid) {
    return out << eid.id;
}


EntityId EntityId::nextID() {
    static uint32_t id = 1;
    EntityId eid(id);
    id++;
    return eid;
}

Entity::Entity(EntityId id) : id_(id) {}

