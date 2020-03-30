#pragma once
#include <util/typealiases.hpp>

namespace ECS {

// Forward delaration
struct GameContext_t;

struct CollisionSystem_t {
    explicit CollisionSystem_t() =default;

    bool update(GameContext_t& g) const;

private :
	bool collide(const Entity_t& e1, const Entity_t& e2) const;
	bool linearOverlap(float x1, float w1, float x2, float w2) const;
	glm::vec3 calculatePosition(const glm::vec3& position, const glm::vec3& length, const glm::vec3& offset) const;
};
}