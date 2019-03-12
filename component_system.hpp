#pragma once

class ComponentSystem {
    public:
        ComponentSystem() = default;
        virtual ~ComponentSystem() = default;

        virtual bool init() = 0;
        virtual void update() = 0;
};
