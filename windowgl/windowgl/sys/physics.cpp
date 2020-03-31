#include <sys/physics.hpp>
#include <util/gamecontext.hpp>
#include <sys/render.hpp>

namespace ECS {

bool PhysicsSystem_t::update(GameContext_t& g) const {
    for (auto& phy : g.getPhysicsComponents()) {
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