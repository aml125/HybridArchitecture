#pragma once

#include <glm\glm\ext\vector_float3.hpp>
#include <game\cmp\physics.hpp>
namespace GM {
	struct Target {
		glm::vec3 position{};
		glm::vec3 orientation{};
	};

	struct Location {
		glm::vec3 position{};
		glm::vec3 orientation{};
	};

	struct DriftOffset {
		glm::vec3 position{};
		glm::vec3 orientation{};
	};
}