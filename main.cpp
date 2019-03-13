#include "ecs.hpp"
#include <iostream>

using namespace std;

struct Test : Component {
    Test(int xx = 0) : x(xx) {}
    int x;
};

int main() {

    ECS::init();

    // create 3 entities
    ECS::createEntity();
    ECS::printState();
    ECS::createEntity();
    ECS::printState();
    EntityId e3 = ECS::createEntity();
    ECS::printState();

    ECS::removeEntity(1);
    ECS::printState();


    e3->createComponent<Test>();
    // auto handle = ECS::createComponent<Test>(e3);
    ECS::printState();
    // ECS::createComponent<Test>(e3, 5);
    // ECS::createComponent<Test>(e3);
    
    /*
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
    */

    ECS::shutdown();
    return 0;
}
