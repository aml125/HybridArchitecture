#pragma once
#include <ecs/man/entitymanager.hpp>

namespace GM {
	struct System_t {
		explicit System_t() = default;

		virtual void update(ECS::EntityManager_t& em) = 0;
	};
}