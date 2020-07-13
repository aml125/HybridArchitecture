#pragma once

#include <cstddef>
#include <vector>
#include <game\sys\system.hpp>
#include <ecs/man/entitymanager.hpp>

namespace GM {

	struct GameManager {
        explicit GameManager();

        bool update();

        void addSystem(System_t& sys) {
            systems.push_back(&sys);
        }

        ECS::EntityManager_t entityMan;
        std::vector<System_t*> systems;
	};
}