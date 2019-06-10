#include <iostream>

#include "ecs.hpp"

using namespace std;

typedef struct A {
    A() = default;
    ~A() { cout << "in A's destructor" << endl; }
    int x;
} A;

int main() {
    Entity e0 = ECS::entity::create();
    Entity e1 = ECS::entity::create();
    Entity e2 = ECS::entity::create();
    Entity e3 = ECS::entity::create();
    Entity e4 = ECS::entity::create();
    Entity e5 = ECS::entity::create();
    
    A* a0 = ECS::component::create<A>(e0);
    a0->x = 0;
    A* a1 = ECS::component::create<A>(e1);
    a1->x = 1;
    A* a2 = ECS::component::create<A>(e2);
    a2->x = 2;
    A* a3 = ECS::component::create<A>(e3);
    a3->x = 3;
    A* a4 = ECS::component::create<A>(e4);
    a4->x = 4;
    A* a5 = ECS::component::create<A>(e5);
    a5->x = 5;

    cout << "Components:" << endl;
    ECS::component::for_each<A>([](A& comp) { cout << comp.x << endl; });
    cout << endl;

    cout << "Entity 2 has component? " << ECS::component::has<A>(e2) << endl;
    cout << "Deleting component A of entity 2..." << endl;
    ECS::component::destroy<A>(e2);
    cout << "Entity 2 has component? " << ECS::component::has<A>(e2) << endl;

    cout << "Components:" << endl;
    ECS::component::for_each<A>([](A& comp) { cout << comp.x << endl; });
    cout << endl;

    cout << "Entity 4 has component? " << ECS::component::has<A>(e4) << endl;
    cout << "Deleting entity 4..." << endl;
    ECS::entity::destroy(e4);
    cout << "Entity 4 has component? " << ECS::component::has<A>(e4) << endl;
    
    cout << "Components:" << endl;
    ECS::component::for_each<A>([](A& comp) { cout << comp.x << endl; });
    cout << endl;

    return 0;
}
