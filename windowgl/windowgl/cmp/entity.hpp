#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <cmp/physics.hpp>
#include <cmp/model.hpp>
#include <cmp/boxcollider.hpp>

namespace ECS {

struct Entity_t {
	explicit Entity_t(const std::string modelPath) 
		: model { modelPath.c_str() }
	{};
    
	//Components
    PhysicsComponent_t* phy {};
	Model_t model;
	BoxCollider_t collider{};

	const Model_t& getModel() const { return model; };
    std::size_t entityID { ++nextID };
    inline static std::size_t nextID { 0 };
};
}