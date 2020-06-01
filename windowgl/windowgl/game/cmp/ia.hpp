#pragma once
#include <cstdio>
#include <ecs/cmp/component.hpp>
#include <glm\glm\ext\vector_float3.hpp>

namespace GM {
	struct Target {
		glm::vec3 position{};
		glm::vec3 orientation{};
	};

	struct IA_t : ECS::Component_t {
		explicit IA_t(std::size_t eid) 
			:	Component_t{eid}
		{}

		Target target{};
		float maxAcceleration = 2;
		float maxSpeed = 5;
		float targetRadius = 3;
		float slowRadius = 7;
		float timeToTarget = 0.1;
		unsigned int slotNumber;
	};
}