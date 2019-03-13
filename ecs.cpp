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
    std::unordered_map<std::type_index, std::vector<uint32_t>> components_generation_;
    std::unordered_map<std::type_index, std::deque<uint32_t>> components_free_list_;
    // std::unordered_map<std::type_index, std::unordered_map<EntityId, uint8_t>> newComponents_;
    
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

    void printState() {
        using std::cout;
        cout << "Entities (" << entities_.size() << "):\n";
        cout << "---------------------------\n";
        for (const auto& e_tup : entities_) {
            cout << "Map id = " << e_tup.first << ", entity = \n";
            cout << e_tup.second << "\n";
        }

        cout << "Components lists:\n";
        cout << "---------------------------\n";
        for (const auto& c : components_) {
            cout << "Type = '" << demangle(c.first.name()) << "', num = " << c.second.size() << "\n";
        }

        cout << "Components lists:\n";
        cout << "---------------------------\n";
        for (const auto& c : components_) {
            cout << "Type = '" << demangle(c.first.name()) << "', num = " << c.second.size() << "\n";
        }

        cout << "\nComponents free lists:\n";
        cout << "---------------------------\n";
        for (const auto& c : components_free_list_) {
            cout << "Type = '" << demangle(c.first.name()) << "', num = " << c.second.size() << "\nFree spots = ";
            for (const auto& f : c.second)
                cout << f << " ";
            cout << "\n";
        }

        cout << "\nComponents generation:\n";
        cout << "---------------------------\n";
        for (const auto& c : components_generation_) {
            cout << "Type = '" << demangle(c.first.name()) << "', num = " << c.second.size() << "\nGeneration numbers = ";
            for (const auto& g : c.second)
                cout << g << " ";
            cout << "\n";
        }
        cout << std::endl << std::endl;
    }

} // namespace ECS
