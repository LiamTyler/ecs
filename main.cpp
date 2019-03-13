#include "ecs.hpp"
#include <iostream>

using namespace std;

struct Test : Component {
    Test(int xx = 0) : x(xx) {}
    int x;
};

int main() {

    ECS::init();

    ECS::createEntity();
    ECS::createEntity();
    ECS::createEntity();
    ECS::createEntity();
    ECS::createEntity();
    ECS::createEntity();
    auto e = ECS::createEntity();

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
    auto handle = ECS::createComponent<Test>(e);
    ECS::createComponent<Test>(e, 5);
    ECS::createComponent<Test>(e);
    size_t numTests;
    auto tests = ECS::getComponents<Test>(numTests);
    for (size_t i = 0; i < numTests; ++i) {
        cout << "x: " << tests[i].x << endl;
        cout << "entity: " << tests[i].entity << endl;
        tests[i].x = i;
    }
    handle->x = 10;
    for (size_t i = 0; i < numTests; ++i) {
        cout << tests[i].x << endl;
        tests[i].x = i;
    }

    ECS::shutdown();
    return 0;
}
