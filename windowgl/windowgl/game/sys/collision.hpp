#pragma once
#include <ecs/man/entitymanager.hpp>
#include <game\cmp\boxcollider.hpp>
#include <game\cmp\physics.hpp>
#include <game/sys/system.hpp>

namespace GM {

// Forward delaration
struct GameContext_t;

struct CollisionSystem_t : System_t{
    explicit CollisionSystem_t() =default;

    void update(ECS::EntityManager_t& g) override;

private :
	bool collide(const PhysicsComponent_t& phy1, const PhysicsComponent_t& phy2, const BoxCollider_t& e1, const BoxCollider_t& e2) const;
	bool linearOverlap(float x1, float w1, float x2, float w2) const;
	glm::vec3 calculatePosition(const glm::vec3& position, const glm::vec3& length, const glm::vec3& offset) const;
	void modifySpeedAndVelocityOnCollision(PhysicsComponent_t& phy1, PhysicsComponent_t& phy2,
		const BoxCollider_t& coll1, const BoxCollider_t& coll2) const;
};
}