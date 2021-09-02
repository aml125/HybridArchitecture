#pragma once
#include <ecs/man/entitymanager.hpp>
#include <game/util/opencl-utils.hpp>

namespace GM {
	struct System_t {
		explicit System_t() = default;

		virtual void update(ECS::EntityManager_t& em) = 0;
		//inline static ocl_args_d_t ocl{std::string("Intel"), true};
	};
}