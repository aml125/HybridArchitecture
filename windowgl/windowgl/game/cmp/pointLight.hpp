#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shader.h>
#include <stb_image.h>
#include <memory>
#include <game/cmp/boxrenderer.hpp>

namespace GM {
	struct PointLight_t {
		explicit PointLight_t(const glm::vec3& position,
			const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular);

		explicit PointLight_t(const glm::vec3& position,
			const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular,
			float constantDecrease, float linearDecrease, float quadraticDecrease);

		glm::vec3 position;
		glm::vec3 ambient;
		glm::vec3 specular;
		glm::vec3 diffuse;

		//Decrease intensity constants
		const float constantDecrease = 1.0f;
		const float linearDecrease = 0.09;
		const float quadraticDecrease = 0.032;

		BoxRenderer_t boxRenderer;

	private:
		void bindVertexArray();
	};
}