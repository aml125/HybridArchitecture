#include <ecs/man/entitymanager.hpp>
#include <algorithm>
#include <GLFW\glfw3.h>



namespace ECS {

EntityManager_t::EntityManager_t() {
    m_Entity.reserve(kNUMINITALLENTITIES);
}

Entity_t& EntityManager_t::createEntity() {
    Entity_t& e = m_Entity.emplace_back();
    return e;
}

}