#pragma once

#include <glm\glm\ext\vector_float3.hpp>

namespace ECS {
	struct BoxCollider_t {
		explicit BoxCollider_t() = default;
		glm::vec3 position{};
		glm::vec3 length{}; //Width height profound
	};
}