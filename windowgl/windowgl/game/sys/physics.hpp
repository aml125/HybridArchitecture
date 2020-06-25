#pragma once
#include <ecs/man/entitymanager.hpp>
#include <game\cmp\physics.hpp>
#include <game/sys/system.hpp>
#include <game/util/timemeasure.hpp>

namespace GM {

struct PhysicsSystem_t : System_t {
    explicit PhysicsSystem_t() =default;

    void update(ECS::EntityManager_t& g) override;
    void moveObject(PhysicsComponent_t& phy) const; //TODO Change to private
private:
    void aplyGravity(PhysicsComponent_t& cmp) const;
    TimeMeasure tm{};
    bool firstTime = true;
};
}