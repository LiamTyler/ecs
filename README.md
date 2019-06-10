# ECS

## Description
My first stab at an ECS for my game engine.

## Features
- Components can be any type, no base component class
- Tightly packed arrays of components. Uses a sparse set for indexing, so one level of indirection
- Iteration over the tightly packed arrays
- Can and and remove components without invalidating iterators

## Missing features (and potential fixes)
- Adding and removing components invalidates raw pointers to data (use a ComponentHandle / index class)
- Given a component, no way to get the corresponding entity (use a ComponentHandle / index class)
- No easy / efficient way to query for all entities with a set of components (not sure if desired)
- Not efficient to check which components an entity has (could use a bitset)
