#pragma once

#include <glad/glad.h>
#include <glm\glm\ext\vector_float3.hpp>
#include <glm\glm\ext\vector_float2.hpp>

namespace ECS {
	struct Vertex_t {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};
}