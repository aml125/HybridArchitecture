#include <game/sys/physics.hpp>
#include <game/sys/render.hpp>

namespace GM {

    bool PhysicsSystem_t::update(ECS::EntityManager_t& g) const {
    for (auto& phy : g.getComponents<PhysicsComponent_t>()) {
        if (phy.gravity) {
            aplyGravity(phy);
        }
        moveObject(phy);
    }
    return true;
}

void PhysicsSystem_t::aplyGravity(PhysicsComponent_t& cmp) const {
    cmp.speed += cmp.gravityAcell * RenderSystem_t::deltaTime;
}
void PhysicsSystem_t::moveObject(PhysicsComponent_t& phy) const
{
    phy.position += phy.speed * RenderSystem_t::deltaTime;
}
}