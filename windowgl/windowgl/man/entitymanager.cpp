#include <man/entitymanager.hpp>
#include <algorithm>
#include <cmp/physics.hpp>
#include <GLFW\glfw3.h>



namespace ECS {

EntityManager_t::EntityManager_t() {
    m_Entity.reserve(kNUMINITALLENTITIES);
}

void EntityManager_t::createEntity(glm::vec3 pos, std::string path) {
    Entity_t& e = m_Entity.emplace_back(path);
    auto& ph = m_components.createPhysycsComponent();
    e.phy = &ph;
	ph.position = pos;
	ph.speed.x = ph.speed.y = ph.speed.z = 0;

}

}