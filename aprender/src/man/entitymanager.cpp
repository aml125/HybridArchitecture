#include <man/entitymanager.hpp>
#include <algorithm>
#include <cmp/physics.hpp>

namespace ECS {

EntityManager_t::EntityManager_t() {
    m_Entity.reserve(kNUMINITALLENTITIES);
}

void EntityManager_t::createEntity(uint32_t x, uint32_t y, std::string filename) {
    Entity_t& e = m_Entity.emplace_back(filename, 1, 1);
    auto& ph = m_components.createPhysycsComponent();
    e.phy = &ph;
    ph.x = x; ph.y = y;
    ph.vx = 1; ph.vy = 1;
    //Set color in all elements of sprite
    // std::fill(begin(e.sprite), end(e.sprite), color);
}

}