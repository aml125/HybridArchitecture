#include <sys/physics.hpp>
#include <util/gamecontext.hpp>
#include <sys/render.hpp>

namespace ECS {

bool PhysicsSystem_t::update(GameContext_t& g) const {
    for (auto& phy : g.getPhysicsComponents()) {
		phy.position += phy.speed * RenderSystem_t::deltaTime;
    }
    return true;
}
}