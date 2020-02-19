#include <sys/physics.hpp>
#include <util/gamecontext.hpp>

namespace ECS {

bool PhysicsSystem_t::update(GameContext_t& g) const {
    for (auto& phy : g.getPhysicsComponents()) {
        phy.x += phy.vx;
        phy.y += phy.vy;
    }
    return true;
}
}