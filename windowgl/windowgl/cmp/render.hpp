#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shader.h>
#include <glad\glad.h>
#include <stb_image.h>
#include <memory>

namespace ECS {
	struct RenderComponent_t {
		explicit RenderComponent_t(glm::vec3 position);

		std::unique_ptr<float[]> vertices;
		std::unique_ptr<float[]> texCoords;

		glm::vec3 position;
		unsigned int VBO;
		unsigned int VAO;
		std::unique_ptr<unsigned int[]> textures;
		float angle = 20.0f;
	};
}