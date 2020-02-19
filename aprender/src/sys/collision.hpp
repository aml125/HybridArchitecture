#pragma once
#include <util/typealiases.hpp>

namespace ECS {

// Forward delaration
struct GameContext_t;

struct CollisionSystem_t {
    explicit CollisionSystem_t() =default;

    bool update(GameContext_t& g) const;
};
}