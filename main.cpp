#include "ecs.hpp"
#include <iostream>

int main() {

    ECS e;
    e.init();

    // Entity e;
    e.createEntity();
    e.createEntity();
    e.createEntity();
    e.createEntity();
    e.createEntity();
    e.createEntity();
    e.createEntity();

    for (const auto& entity : e.entities_)
        std::cout << entity.second.getId() << std::endl;


    std::cout << std::endl;

    std::cout << "4 alive? " << e.entityIsAlive(4) << std::endl;
    e.removeEntity(4);
    e.removeEntity(4);
    std::cout << "4 alive? " << e.entityIsAlive(4) << std::endl;

    for (const auto& entity : e.entities_)
        std::cout << entity.second.getId() << std::endl;


    e.shutdown();
    return 0;
}
