#pragma once
#include <ecs/man/entitymanager.hpp>
#include <game\cmp\physics.hpp>

namespace GM {

// Forward delaration
struct GameContext_t;

struct PhysicsSystem_t {
    explicit PhysicsSystem_t() =default;

    bool update(ECS::EntityManager_t& g);

private:
    void aplyGravity(PhysicsComponent_t& cmp) const;
    void moveObject(PhysicsComponent_t& phy) const;
    bool firstTime = true;
};
}