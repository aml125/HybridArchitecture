#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <cmp/physics.hpp>
#include <cmp/render_cmp.hpp>

namespace ECS {

struct Entity_t {
	explicit Entity_t(glm::vec3 position);
	explicit Entity_t(const Entity_t& other);
    
	//Components
    PhysicsComponent_t* phy {};
	RenderComponent_t ren;

    std::size_t entityID { ++nextID };
    inline static std::size_t nextID { 0 };
};
}