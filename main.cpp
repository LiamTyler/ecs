#include "ecs.hpp"
#include <iostream>

using namespace std;

struct Test {
    int x = 0;
};

int main() {

    ECS::init();

    // Entity e;
    ECS::createEntity();
    ECS::createEntity();
    ECS::createEntity();
    ECS::createEntity();
    ECS::createEntity();
    ECS::createEntity();
    ECS::createEntity();

    for (const auto& entity : ECS::entities_)
        std::cout << entity.second.getId() << std::endl;


    std::cout << std::endl;

    std::cout << "4 alive? " << ECS::entityIsAlive(4) << std::endl;
    ECS::removeEntity(4);
    ECS::removeEntity(4);
    std::cout << "4 alive? " << ECS::entityIsAlive(4) << std::endl;

    for (const auto& entity : ECS::entities_)
        std::cout << entity.second.getId() << std::endl;

    cout << "comp size: " << ECS::components_[typeid(Test)].size() << endl;
    ECS::createComponent<Test>();
    ECS::createComponent<Test>();
    ECS::createComponent<Test>();
    size_t numTests;
    auto tests = ECS::getComponents<Test>(numTests);
    for (size_t i = 0; i < numTests; ++i) {
        cout << tests[i].x << endl;
        tests[i].x = i;
    }
    for (size_t i = 0; i < numTests; ++i) {
        cout << tests[i].x << endl;
        tests[i].x = i;
    }

    ECS::shutdown();
    return 0;
}
