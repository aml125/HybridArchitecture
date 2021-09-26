#pragma once

#include <cstddef>
#include <vector>
#include <game\sys\system.hpp>
#include <ecs/man/entitymanager.hpp>
#include <game/sys/render.hpp>

namespace GM {

	struct GameManager {
        explicit GameManager(RenderSystem_t* ren);

        bool update();

        void addSystem(System_t& sys) {
            systems.push_back(&sys);
        }

        ECS::EntityManager_t entityMan;
        RenderSystem_t* render{};

    private:
        std::vector<System_t*> systems;
        void sincronize();

        TimeMeasure tm{};
        
	};
}