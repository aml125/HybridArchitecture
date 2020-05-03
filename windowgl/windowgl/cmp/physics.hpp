#pragma once

#include <glm\glm\ext\vector_float3.hpp>
#include <cmp/component.hpp>

namespace ECS {

    struct PhysicsComponent_t : Component_t{
        explicit PhysicsComponent_t(unsigned int eid)
            : Component_t{ eid }
        {};
        
		glm::vec3 position{};
		glm::vec3 speed{};
        glm::vec3 scale{1, 1, 1};
        glm::vec3 gravityAcell{ 0, -0.25f, 0 };
        bool gravity = false;
    };
}