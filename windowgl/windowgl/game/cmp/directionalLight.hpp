#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shader.h>
#include <glad\glad.h>
#include <stb_image.h>
#include <memory>

namespace GM {
	struct DirectionalLight_t {
		explicit DirectionalLight_t(const glm::vec3& direction, 
			const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular);
		
		glm::vec3 direction;
		glm::vec3 ambient;
		glm::vec3 specular;
		glm::vec3 diffuse;


		//float vertices[108] = {
		//	// positions        
		//	-0.5f, -0.5f, -0.5f,
		//	 0.5f, -0.5f, -0.5f,
		//	 0.5f,  0.5f, -0.5f,
		//	 0.5f,  0.5f, -0.5f,
		//	-0.5f,  0.5f, -0.5f,
		//	-0.5f, -0.5f, -0.5f,

		//	-0.5f, -0.5f,  0.5f,
		//	 0.5f, -0.5f,  0.5f,
		//	 0.5f,  0.5f,  0.5f,
		//	 0.5f,  0.5f,  0.5f,
		//	-0.5f,  0.5f,  0.5f,
		//	-0.5f, -0.5f,  0.5f,

		//	-0.5f,  0.5f,  0.5f,
		//	-0.5f,  0.5f, -0.5f,
		//	-0.5f, -0.5f, -0.5f,
		//	-0.5f, -0.5f, -0.5f,
		//	-0.5f, -0.5f,  0.5f,
		//	-0.5f,  0.5f,  0.5f,

		//	 0.5f,  0.5f,  0.5f,
		//	 0.5f,  0.5f, -0.5f,
		//	 0.5f, -0.5f, -0.5f,
		//	 0.5f, -0.5f, -0.5f,
		//	 0.5f, -0.5f,  0.5f,
		//	 0.5f,  0.5f,  0.5f,

		//	-0.5f, -0.5f, -0.5f,
		//	 0.5f, -0.5f, -0.5f,
		//	 0.5f, -0.5f,  0.5f,
		//	 0.5f, -0.5f,  0.5f,
		//	-0.5f, -0.5f,  0.5f,
		//	-0.5f, -0.5f, -0.5f,

		//	-0.5f,  0.5f, -0.5f,
		//	 0.5f,  0.5f, -0.5f,
		//	 0.5f,  0.5f,  0.5f,
		//	 0.5f,  0.5f,  0.5f,
		//	-0.5f,  0.5f,  0.5f,
		//	-0.5f,  0.5f, -0.5f
		//};

		//unsigned int VBO = 0;
		//unsigned int VAO = 0;
		float angle = 20.0f;
	};
}