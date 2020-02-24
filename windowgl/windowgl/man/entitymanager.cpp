#include <man/entitymanager.hpp>
#include <algorithm>
#include <cmp/physics.hpp>
#include <GLFW\glfw3.h>



namespace ECS {

EntityManager_t::EntityManager_t() {
    m_Entity.reserve(kNUMINITALLENTITIES);
}

void EntityManager_t::createEntity(glm::vec3 pos) {
    Entity_t& e = m_Entity.emplace_back(pos);
    auto& ph = m_components.createPhysycsComponent();
    e.phy = &ph;
    ph.x = 0; ph.y = 0;
    ph.vx = 1; ph.vy = 1;
    //Set color in all elements of sprite
    // std::fill(begin(e.sprite), end(e.sprite), color);
}

}