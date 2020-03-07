#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
//#include <cmp/physics.hpp>
#include <cmp/model.hpp>

namespace ECS {

//struct Model {
//	explicit Model(const char* str) {
//		x = str[0];
//	}
//
//	char x;
//};

struct Entity_t {
	explicit Entity_t(const std::string modelPath) 
		: model { modelPath.c_str() }
	{};
    
	//Components
    //PhysicsComponent_t* phy {};
	
	Model model;
	const Model& getModel() const { return model; };
    std::size_t entityID { ++nextID };
    inline static std::size_t nextID { 0 };
};
}