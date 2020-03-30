#pragma once

#include <glm\glm\ext\vector_float3.hpp>

namespace ECS {
	struct BoxCollider_t {
		explicit BoxCollider_t() = default;
		glm::vec3 position{}; //Center of the box
		glm::vec3 length{}; //Width height profound, tamaño del lado de la caja
		glm::vec3 offset{};
	};
}