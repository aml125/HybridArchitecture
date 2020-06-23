#pragma once
#include <game/icmp/datatypes.hpp>
#include <cstdio>
#include <ecs/cmp/component.hpp>
#include <glm\glm\ext\vector_float3.hpp>

namespace GM {
	struct IA_t : ECS::Component_t {
		explicit IA_t(std::size_t eid)
			: Component_t{ eid }
		{}

		Target target{};
		float maxAcceleration = 10;
		float maxSpeed = 10;
		float targetRadius = 0.1;
		float slowRadius = 2;
		float timeToTarget = 0.5;
		unsigned int slotNumber = 999999;
		bool isAnchorPoint = false;
		unsigned int patternNumber = 999999;
		StateTypes state = WAIT;
		unsigned int enemyID;
	};
}