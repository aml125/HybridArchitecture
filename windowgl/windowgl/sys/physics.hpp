#pragma once
#include <util/typealiases.hpp>

namespace ECS {

// Forward delaration
struct GameContext_t;

struct PhysicsSystem_t {
    explicit PhysicsSystem_t() =default;

    bool update(GameContext_t& g) const;

private:
    void aplyGravity(PhysicsComponent_t& cmp) const;
    void moveObject(PhysicsComponent_t& phy) const;
};
}