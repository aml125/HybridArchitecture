#pragma once

#include <glm\glm\ext\vector_float3.hpp>
#include <ecs\cmp\component.hpp>

namespace GM {
	struct BoxCollider_t : ECS::Component_t {
		explicit BoxCollider_t(std::size_t eid) 
			: Component_t{eid}
		{}
		glm::vec3 length{}; //Width height profound, tamaño del lado de la caja
		glm::vec3 offset{};
	};
}