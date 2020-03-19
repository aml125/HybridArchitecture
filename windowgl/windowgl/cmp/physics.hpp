#pragma once

#include <glm\glm\ext\vector_float3.hpp>

namespace ECS {

    struct PhysicsComponent_t {
        explicit PhysicsComponent_t() = default;
        
		glm::vec3 position{};
		glm::vec3 speed{};
    };
}